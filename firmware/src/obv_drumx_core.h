#include "obv_drum_sine.h"
/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
 * Copyright (C) 2026 Juan Ignacio Antonucci (Juan Jiant)
 * JIANT (FELUCCA TONIC): DRUM-X, after the idea of Sonic Charge's Microtonic (a tuned oscillator with a pitch
 * envelope and FM, a filtered noise generator, their envelopes, two patches and a MORPH between them), built anew
 * small and in fixed point for the FM-1. */
/* A DRUM-X sound (a lane of a kit) is dx_lane_t, 11 bytes: the mode and five values for each of its patches A and B:
 *   PITCH  the oscillator, MIDI 24..119 (33 Hz .. 7.9 kHz), 3/4 semitone a step
 *   PMOD   the pitch envelope: up to +4 octaves at the hit, falling with a sixth of the decay's tau (1..40 ms)
 *   DECAY  the amplitude's tau, 3 ms .. 1.5 s (exponential)
 *   NOISE  the mix: 0 the oscillator only .. 127 the noise only
 *   COLOR  the oscillator's FM index (SINE: its drive) and the noise filter's cutoff (30 Hz .. 16 kHz)
 * The mode: the oscillator (SINE, FM at 1:1, METAL at 1:1.47, BELL at 1:2.76), the noise filter (LP, BP, HP), the
 * noise's envelope (as the oscillator's, or SNAP: a quarter of its tau); (JIANT 0.4, the mode's free bits 5..7) the
 * pitch modulation, as Microtonic's: DECAY (the envelope above), LONG (its tau half the decay's, up to 250 ms: the
 * 808 boom), NOISE (a random pitch each block, scaled by the envelope: glitch, grit), SINE (a sine at 1/8 of the
 * pitch, scaled by the envelope: wobble, laser); and DRIVE (the oscillator x3 into a soft clip: dense kicks, fat toms).
 * MORPH (0 A .. 127 B) mixes the two patches value by value, read every block: it may move while a hit rings.
 * Envelopes per block (Q30 values, Q16 factors, dv_kb), ramped linearly inside it; the pitch ramps too. A hit
 * that has rung out (-84 dB) stops computing (live 0) and writes zeros. Only 32-bit products. */
/* Obervave adaptation of JIANT DRUM-X, pinned e924445ae0e7c64636f15bd0758418ee312ce7ee.
 * CTL 16 (not 32), 44.1 kHz output / 22.05 kHz drum core. Per-track context
 * passed explicitly; no JIANT UI, buses, sequencer, words or sample layers.
 * Wider intermediate products avoid overflow at extreme settings. */
#include "obv_drum_tables.h"
_Static_assert(CTL==16u && FS==44100u,"DRUM-X block/rate adaptation must be updated");
static int32_t dx_mulq15(int32_t a,int32_t b){return (int32_t)(((int64_t)a*b)>>15);}
static int32_t dx_mulq16(int32_t a,uint32_t b){return (int32_t)(((int64_t)a*b)>>16);}
static uint32_t dx_noise32(int32_t *p){uint32_t q=(uint32_t)*p;uint32_t r=xorshift(&q);*p=(int32_t)q;return r;}
static uint32_t dx_pitch_inc(int32_t p16){return pitch_inc(p16*16);}
/* Both projects generate TANH_Q15 as tanh(i/256*2). */
static int32_t dx_softclip(int32_t x){return softclip(x);}
typedef struct {int32_t a1,a2,a3;} dx_svf_t;
static void dx_svf_coef(dx_svf_t *c,int32_t cut,int32_t k) {
    cut=clampi(cut,0,127*256);int32_t i=cut>>8,g=DX_SVF_G[i];
    if(i<127)g+=((DX_SVF_G[i+1]-g)*(cut&255))>>8;
    int32_t den=4096+((g*(g+k))>>12);
    c->a1=(int32_t)((4096u<<13)/(uint32_t)den);
    c->a2=c->a1*g>>12;c->a3=c->a2*g>>12;
}
enum { DV_KICK, DV_SNARE, DV_CLAP, DV_HATC, DV_HATO, DV_TOM, DV_RIM, DV_BELL, DV_NLANE };   /* the kit's lanes */
enum { DXW_SINE, DXW_FM, DXW_METAL, DXW_BELL };
enum { DXF_LP, DXF_BP, DXF_HP };
#define DX_MODE(w, f, snap) ((uint8_t)((w) | (f) << 2 | (snap) << 4))
enum { DXM_DECAY, DXM_LONG, DXM_NOISE, DXM_SINE };       /* (JIANT 0.4) the pitch modulation: mode bits 5..6 */
#define DX_DRIVE 0x80u                                   /* .. the oscillator driven: bit 7 */
enum { DXP_PITCH, DXP_PMOD, DXP_DECAY, DXP_NOISE, DXP_COLOR, DXP_N };
typedef struct {
    uint8_t mode;
    uint8_t a[DXP_N], b[DXP_N];                          /* 0..127 */
} dx_lane_t;
typedef struct {                                         /* a hit */
    uint32_t ph, ph2, inc;                               /* the phases; the last block's end increment */
    int32_t ea, en, ep;                                  /* amplitude, noise (Q30), pitch (Q15) envelopes */
    int32_t ic1, ic2, rng;                               /* the noise filter's state, the noise */
    int32_t mo;                                          /* WARP: the modulator's last output (its feedback) */
    uint32_t ph3;                                        /* (JIANT 0.4) the SINE pitch modulation's phase */
    int32_t lp;                                          /* (JIANT 0.5) the output's one-pole low-pass state */
    int32_t hl;                                          /* (0.6.17) the last half-rate sample (dx_run's upsampling) */
    int32_t b1, b2;                                      /* (0.6.36) TONE's band-pass state */
    uint32_t ph4;                                        /* (0.6.49) LAYER's oscillator phase */
    uint8_t active, trig, choke;
    uint16_t hpos, hsum, hseed;                          /* (0.7.9) CLAP's HANDS: half-rate samples since the hit, the
                                                          * hands come in so far (Q12, over the hit's own), this hit's dice */
} dx_voice_t;

/* the oscillator's modulator ratios (Q12), by wave */
static const uint16_t DX_RATIO[4] = {4096, 4096, 6021, 11305};

/* 2^(x / 4096), 0 <= x < 16 * 4096, Q16 (a cubic for the fraction: within 0.01 %) */
static uint32_t dv_exp2(uint32_t x)
{
    uint32_t f = x & 4095u, m;
    m = 65536u + ((f * (45594u + ((f * (14851u + ((f * 5092u) >> 12))) >> 12))) >> 12);
    return m << (x >> 12);
}
/* tau (us) -> per-block decay, Q16: e^(-CTL / (tau fs)), to the fourth order */
static uint32_t dv_kb(uint32_t us)
{
    uint32_t x = 23777233u / (us | 1u), x2, x3, x4, k;
    if (x > 46000u)
        x = 46000u;
    x2 = (x * x) >> 16;
    x3 = (x2 * x) >> 16;
    x4 = (x3 * x) >> 16;
    k = 65536u - x + (x2 >> 1) - x3 / 6u + x4 / 24u;
    return k > 65535u ? 65535u : k;
}

static void dx_trigger(dx_voice_t *v) { v->trig = 1; v->choke = 0; }
static void dx_choke(dx_voice_t *v) { if (v->active) v->choke = 1; }

/* a decay value (0..127) -> its tau in us: 3 ms x 2^(9 d / 127) */
static uint32_t dx_tau(uint32_t d) { return ((dv_exp2(d * 9u * 4096u / 127u) >> 8) * 3000u) >> 8; }

/* (0.6.49) LAYER's oscillator: a triangle (no table, a few operations; its odd partials add to the clang) */
static inline int32_t dx_tri(uint32_t ph)
{
    int32_t s = (int32_t)(ph >> 15);                     /* 0 .. 131071 */
    return s < 65536 ? s - 32768 : 98303 - s;
}

/* (0.6.49) the oscillator's loop with LAYER (dx_run's FM ENV / FEEDBK loop and a triangle at sqrt 2 of it, frequency
 * modulated by it, its index up as LAYER's square; up to ~70 % of the mix): a function of its own (dx_run's loops as
 * they were) */
static __attribute__((noinline)) void dx_layer(dx_voice_t *v, int32_t *y, uint32_t n, uint32_t *php, uint32_t *ph2p,
                                               int32_t *op, int32_t inc, int32_t dinc, int32_t ga, int32_t dga, int32_t ix,
                                               int32_t dix, uint32_t ratio, int32_t fbk, int32_t lay)
{
    uint32_t i, ph = *php, ph2 = *ph2p, ph4 = v->ph4;
    int32_t o = *op, lx = lay * lay * 600, lm = lay * 3;
    if (ix || dix)
        for (i = 0; i < n; i++, inc += dinc, ga += dga, ix += dix) {
            int32_t o2;
            o = drum_sine(ph + (uint32_t)(drum_sine(ph2) * ix) * 512u + (uint32_t)(o * fbk));
            o2 = dx_tri(ph4 + ((uint32_t)o * (uint32_t)lx));
            y[i] = dx_mulq15(o + (((o2 - o) * lm) >> 6), ga >> 8);
            ph += (uint32_t)inc << 4;
            ph2 += ((uint32_t)inc >> 8) * ratio;
            ph4 += ((uint32_t)inc >> 6) * 1448u;         /* (x 22.6: x 16 the step, sqrt 2) */
        }
    else                                                 /* (no FM on it: the modulator not run) */
        for (i = 0; i < n; i++, inc += dinc, ga += dga) {
            int32_t o2;
            o = drum_sine(ph + (uint32_t)(o * fbk));
            o2 = dx_tri(ph4 + ((uint32_t)o * (uint32_t)lx));
            y[i] = dx_mulq15(o + (((o2 - o) * lm) >> 6), ga >> 8);
            ph += (uint32_t)inc << 4;
            ph4 += ((uint32_t)inc >> 6) * 1448u;
        }
    *php = ph;
    *ph2p = ph2;
    *op = o;
    v->ph4 = ph4;
}

/* one block of lane L's hit v into y (n <= CTL): Q15, peaks near full scale. morph 0..127; tune16 1/16 semitones;
 * dofs / cofs / nofs move DECAY / COLOR / NOISE (-64..63). warp (DRUM's FOLD since JIANT 0.5, was WARP, 0..127): the
 * oscillator through a wavefolder after its envelope, x1 .. x9 into three folds at +-T: the hit's loud start folds
 * into bright partials, its tail unfolds back to the plain wave (a West Coast drum); 0: the patch as it is.
 * fm (DRUM's FM ENV, JIANT 0.6.36; was FM, 0..127): how much FM the hit strikes with, its index following the pitch
 * envelope (1..40 ms) down to the lane's own: an attack's beater / click / metal edge, the tail as the kit has it
 * (the old FM, a fixed harmonic FM over the whole hit, muddied every sound). The oscillator and the noise each run their own loop (the wave and the
 * filter picked once a block), skipped when the mix leaves them out; gains and the pitch step per sample */
/* (JIANT 0.6.36) EDIT > TONE, the kit's (project.c keeps it in the kit's reserved byte 7: 0 in older projects, off):
 * low nibble BP, a band-pass on the metal sounds (the hats, the rim, the cowbell), 1..15 its centre ~1.5 .. 9 kHz;
 * high nibble FEEDBK, every sound's oscillator fed back into its own phase (1..15: warm, growl, grit, noise) */

/* (JIANT 0.7.9) KIT 3 > HANDS, the kit's (project.c keeps it in the high nibble of the kit's reserved byte 0, the
 * mute groups' low nibble: 0 in older projects, off): 1..15 more hands on the CLAP, each the hit's noise struck again
 * at a moment of its own (~3 .. 45 ms after it, the dice thrown again each hit: no tempo in it) and at a level of its
 * own, then dying as the hit does: a few people clapping, then a crowd. 0: the clap as it was */


/* (JIANT 0.6.49) EDIT > LAYER, each sound's (the kit's: project.c keeps it, FUNC's 'D' 'L' block; 0 before it): low
 * nibble LAYER, a second oscillator (a triangle) at sqrt 2 of the pitch (the most out of tune with it there is) frequency-modulated
 * by the first, more as it goes up (1..15: a metal edge .. a clang with no note: the sound melts into the rhythm and
 * stays out of the harmony); high nibble FEEDBK, the sound's own oscillator fed back (on top of TONE's, the kit's) */
#define DX_LAY_INIT {0, 6, 0, 9, 9, 0, 0, 11}            /* (the factory kit's: DX_LAY_DEF) */

#define DX_LAY_L(x) ((uint32_t)(x) & 15u)
#define DX_LAY_FB(x) ((uint32_t)(x) >> 4)
#define DX_TONE_BP(t) ((uint32_t)(t) & 15u)
#define DX_TONE_FB(t) ((uint32_t)(t) >> 4)
/* (0.7.9) CLAP's HANDS, once a half-rate block (~0.7 ms: each hand's onset on its block, its rise the block's ramp):
 * the noise's gain at the block's start and end, gn and *g1 (Q23), times 1 + the hands in. Hand k strikes at its own
 * moment (this hit's dice, v->hseed, 3 .. 45 ms), at its own level, then decays as the hit: in the sum it is the hit's
 * envelope times its level over where that envelope was when it struck, so one sum carries them all (v->hsum, Q12) */
static __attribute__((noinline)) int32_t dx_hands_step(dx_voice_t *v, uint32_t n, int32_t gn, int32_t *g1, int32_t en1, uint8_t dx_hands)
{
    uint32_t k, nh = 1u + (uint32_t)dx_hands * 5u / 15u, span = 60u + (uint32_t)dx_hands * 60u, p0 = v->hpos;
    int32_t s0 = v->hsum, s = s0, w = 8 + dx_hands;      /* (the hands louder as there are more of them) */
    for (k = 0; k < nh && p0 < 1200u; k++) {             /* (all in by ~54 ms: then the sum alone) */
        uint32_t h = ((uint32_t)v->hseed + k * 0x9E37u) * 2654435761u, on = 66u + (h >> 8) % span;
        if (on >= p0 && on < p0 + n) {
            int32_t f = (int32_t)((1u << 30) / (uint32_t)((en1 >> 12) | 1));   /* 1 / the envelope there, Q12 */
            if (f > 16 << 12) f = 16 << 12;
            s += (((int32_t)(1200u + ((h >> 20) & 2047u)) * w / 23) * f) >> 12;   /* its level, ~0.3 .. 0.8 */
        }
    }
    s = clampi(s, 0, 12 << 12);
    v->hsum = (uint16_t)(s > 65535 ? 65535 : s);
    v->hpos = (uint16_t)(p0 + n < 65535u ? p0 + n : 65535u);
    *g1 = (int32_t)(((int64_t)*g1 * (4096 + s)) >> 12);
    return (int32_t)(((int64_t)gn * (4096 + s0)) >> 12);
}
static __attribute__((noinline)) void dx_run(const dx_lane_t *L, dx_voice_t *v, int32_t morph, int32_t tune16,
                                             int32_t dofs, int32_t cofs, int32_t nofs, int32_t warp, int32_t fm, int32_t *y,
                                             uint32_t n, uint32_t ln, uint8_t dx_tone, uint8_t dx_hands, uint8_t layer)
{
    int32_t q[DXP_N], p16, depth, ea1, en1, ep1, og, ng, ga, dga, gn, dgn, kd, inc, dinc;
    uint32_t i, k, wave = L->mode & 3u, flt = (L->mode >> 2) & 3u, ta, tn, tp, kb, inc1;
    dx_svf_t c;
    uint32_t nf = n;
    int32_t lay = (int32_t)DX_LAY_L(layer), lfb = (int32_t)DX_TONE_FB(dx_tone) + (int32_t)DX_LAY_FB(layer);
    if (!v->active && !v->trig) {
        for (i = 0; i < n; i++)
            y[i] = 0;
        return;
    }
    n >>= 1;                                             /* (0.6.17) at half rate (22 kHz): the hit is low-passed
                                                          * at ~8 kHz below anyway; the block back to full rate at
                                                          * the end, linearly (half the work of the heaviest lanes) */
    morph = clampi(morph, 0, 127);
    for (k = 0; k < DXP_N; k++)
        q[k] = L->a[k] + ((L->b[k] - L->a[k]) * morph) / 127;
    q[DXP_DECAY] = clampi(q[DXP_DECAY] + dofs, 0, 127);
    q[DXP_COLOR] = clampi(q[DXP_COLOR] + cofs, 0, 127);
    q[DXP_NOISE] = clampi(q[DXP_NOISE] + nofs, 0, 127);
    ta = dx_tau((uint32_t)q[DXP_DECAY]);
    tn = (L->mode >> 4) & 1u ? ta >> 2 : ta;
    tp = ((L->mode >> 5) & 3u) == DXM_LONG ? (uint32_t)clampi((int32_t)(ta / 2u), 4000, 250000)
                                           : (uint32_t)clampi((int32_t)(ta / 6u), 1000, 40000);
    p16 = 384 + q[DXP_PITCH] * 12 + tune16;
    depth = q[DXP_PMOD] * 6;
    if (v->trig) {                                       /* the hit: from the top */
        v->trig = 0;
        v->active = 1;
        v->ea = v->en = 1 << 30;
        v->ep = 32767;
        v->ph = v->ph2 = v->ph4 = 0;
        v->ic1 = v->ic2 = 0;
        v->b1 = v->b2 = 0;
        v->mo = 0;
        if (!v->rng)
            v->rng = 0x2545F491;
        v->hpos = v->hsum = 0;
        v->hseed = (uint16_t)(((uint32_t)v->rng * 2654435761u) >> 16);   /* (not drawn from the noise: OFF as it was) */
        v->inc = dx_pitch_inc((uint32_t)clampi(p16 + depth, 0, 2047));
    }
    kb = v->choke ? dv_kb(1500u) : dv_kb(ta);            /* (a choked hit: gone in ~5 ms) */
    ea1 = dx_mulq16(v->ea, kb);
    en1 = dx_mulq16(v->en, v->choke ? kb : dv_kb(tn));
    ep1 = (int32_t)(((uint32_t)v->ep * dv_kb(tp)) >> 16);
    {
        uint32_t pm = (L->mode >> 5) & 3u;
        int32_t off = (depth * ep1) >> 15;              /* DECAY, LONG: the envelope */
        if (pm == DXM_NOISE) {                          /* (JIANT 0.4) a random pitch each block, the envelope's depth */
            int32_t r = v->rng ? v->rng : 0x2545F491;
            off = (off * ((int32_t)dx_noise32(&r) >> 17)) >> 14;
            v->rng = r;
        } else if (pm == DXM_SINE) {                    /* .. a sine at 1/8 of the pitch */
            v->ph3 += v->inc << 2;
            off = (off * drum_sine(v->ph3)) >> 15;
        }
        inc1 = dx_pitch_inc((uint32_t)clampi(p16 + off, 0, 2047));
    }
    ng = q[DXP_NOISE] * 258;
    og = ((32767 - ng) * 11) >> 4;                       /* (x 11 / 16: about the DRUM kits' level) */
    ng = (ng * 11) >> 4;
    ga = dx_mulq15(og, v->ea >> 15) << 8;                   /* the oscillator's gain, Q23, stepped */
    dga = ((dx_mulq15(og, ea1 >> 15) << 8) - ga) / (int32_t)n;
    gn = dx_mulq15(ng, v->en >> 15) << 8;                   /* the noise's */
    {
        int32_t g1 = dx_mulq15(ng, en1 >> 15) << 8;
        if (ln == DV_CLAP && dx_hands && (v->hpos < 1200u || v->hsum))   /* (0.7.9) HANDS: more hands in the noise */
            gn = dx_hands_step(v, n, gn, &g1, en1, dx_hands);
        dgn = (g1 - gn) / (int32_t)n;
    }
    inc = (int32_t)(v->inc >> 4) * 2;                    /* (>> 4: the step fits; x 2: half rate) */
    dinc = ((int32_t)(inc1 >> 4) * 2 - inc) / (int32_t)n;
    if (og) {                                            /* the oscillator */
        uint32_t ph = v->ph;
        int32_t fold = clampi(warp, 0, 127);
        fm = clampi(fm, 0, 127);
        warp = fm || lfb || lay ? 0x100 : 0;            /* (FM ENV, FEEDBK, LAYER: the loop below; the flag stripped there) */
        if (wave == DXW_SINE && q[DXP_COLOR] && !warp) {   /* drive x1 .. x2.5 into the soft clip */
            int32_t g = 4096 + q[DXP_COLOR] * 48;
            for (i = 0; i < n; i++, inc += dinc, ga += dga) {
                y[i] = dx_mulq15(dx_softclip((drum_sine(ph) * g) >> 12), ga >> 8);
                ph += (uint32_t)inc << 4;
            }
        } else if (wave == DXW_SINE && !warp) {
            for (i = 0; i < n; i++, inc += dinc, ga += dga) {
                y[i] = dx_mulq15(drum_sine(ph), ga >> 8);
                ph += (uint32_t)inc << 4;
            }
        } else if (!warp) {                              /* FM: up to ~1.5 cycles of phase */
            uint32_t ph2 = v->ph2, ratio = DX_RATIO[wave];
            int32_t idx = q[DXP_COLOR] * 3;
            for (i = 0; i < n; i++, inc += dinc, ga += dga) {
                y[i] = dx_mulq15(drum_sine(ph + (uint32_t)(drum_sine(ph2) * idx) * 512u), ga >> 8);
                ph += (uint32_t)inc << 4;
                ph2 += ((uint32_t)inc >> 8) * ratio;
            }
            v->ph2 = ph2;
        } else {                                         /* (0.6.36) FM ENV: the FM struck with the hit and gone with
                                                          * the pitch envelope (1..40 ms): a beater, a click, a metal
                                                          * edge on the attack, the tail clean; a SINE gets its octave
                                                          * as the modulator, the other waves their own ratio and
                                                          * COLOR's index under it */
            uint32_t ph2 = v->ph2, ratio = wave == DXW_SINE ? 8192u : DX_RATIO[wave];
            int32_t base = wave == DXW_SINE ? 0 : q[DXP_COLOR] * 3;
            int32_t ix = base + ((fm * 3 * v->ep) >> 15), dix = ((base + ((fm * 3 * ep1) >> 15)) - ix) / (int32_t)n;
            int32_t o = v->mo, fbk = (lfb > 15 ? 15 : lfb) * 4096;   /* (FEEDBK: up to half a turn) */
            if (lay) {                                   /* (0.6.49) LAYER: a second one at sqrt 2, FM'd by the first */
                dx_layer(v, y, n, &ph, &ph2, &o, inc, dinc, ga, dga, ix, dix, ratio, fbk, lay);
            } else
                for (i = 0; i < n; i++, inc += dinc, ga += dga, ix += dix) {
                    o = drum_sine(ph + (uint32_t)(drum_sine(ph2) * ix) * 512u + (uint32_t)(o * fbk));
                    y[i] = dx_mulq15(o, ga >> 8);
                    ph += (uint32_t)inc << 4;
                    ph2 += ((uint32_t)inc >> 8) * ratio;
                }
            v->ph2 = ph2;
            v->mo = o;
        }
        v->ph = ph;
        if (L->mode & DX_DRIVE)                         /* (JIANT 0.4) DRIVE: x3 into the soft clip, the level near */
            for (i = 0; i < n; i++)
                y[i] = (dx_softclip(y[i] * 3) * 22000) >> 15;
        if (fold) {                                      /* (JIANT 0.5) FOLD: after the envelope, so it follows it */
            int32_t g = 16 + (fold * 5 >> 3);            /* (JIANT 0.5: x1 .. x5.9, Q4; was x16.9: aliased hiss) */
            for (i = 0; i < n; i++) {
                int32_t v = clampi((y[i] * g) >> 4, -5 * 16384, 5 * 16384), a;
                for (a = 0; a < 3; a++)
                    v = v > 16384 ? 32768 - v : v < -16384 ? -32768 - v : v;
                y[i] = (v * 3) >> 1;
            }
        }
    } else {
        for (i = 0; i < n; i++)
            y[i] = 0;
    }
    if (ng) {                                            /* the noise through its filter */
        int32_t ic1 = v->ic1, ic2 = v->ic2, rng = v->rng;
        kd = flt == DXF_BP ? 2400 : 6000;                /* (BP a little resonant) */
        dx_svf_coef(&c, (q[DXP_COLOR] + 14) << 8, kd);   /* (+ an octave of the table: the same Hz at half rate) */
/* (one loop per filter: the choice is not made per sample) */
#define DX_NOISE(OUT)                                                                                              \
        for (i = 0; i < n; i++, gn += dgn) {                                                                       \
            int32_t r = (((int32_t)dx_noise32(&rng) >> 17) * 23170) >> 15, v3 = r - ic2, v1, v2;   /* +-11585: 1/sqrt2,  \
                                                          * half rate (the same power in half the band: +3 dB) */      \
            v1 = ((int64_t)c.a1 * ic1 + (int64_t)c.a2 * v3) >> 13;                                                                   \
            v2 = ic2 + (((int64_t)c.a2 * ic1 + (int64_t)c.a3 * v3) >> 13);                                                           \
            ic1 = clampi(2 * v1 - ic1, -150000, 150000);                                                            \
            ic2 = clampi(2 * v2 - ic2, -150000, 150000);                                                            \
            y[i] += dx_mulq15(clampi((OUT) * 2, -32767, 32767), gn >> 8);                                            \
        }

        if (flt == DXF_LP)
            DX_NOISE(v2)
        else if (flt == DXF_BP)
            DX_NOISE(v1)
        else
            DX_NOISE(r - ((kd * v1) >> 12) - v2)
#undef DX_NOISE
        v->ic1 = ic1;
        v->ic2 = ic2;
        v->rng = rng;
    }
    if (DX_TONE_BP(dx_tone) && (ln == DV_HATC || ln == DV_HATO || ln == DV_RIM || ln == DV_BELL)) {   /* (0.6.36) TONE's BP on the metal sounds */
        int32_t b1 = v->b1, b2 = v->b2;
        dx_svf_t cb;
        dx_svf_coef(&cb, (int32_t)(92u + DX_TONE_BP(dx_tone) * 2u) << 8, 1800);   /* (half rate: + an octave) */
        for (i = 0; i < n; i++) {
            int32_t v3 = y[i] - b2, v1 = ((int64_t)cb.a1 * b1 + (int64_t)cb.a2 * v3) >> 13, v2 = b2 + (((int64_t)cb.a2 * b1 + (int64_t)cb.a3 * v3) >> 13);
            b1 = clampi(2 * v1 - b1, -150000, 150000);
            b2 = clampi(2 * v2 - b2, -150000, 150000);
            y[i] = clampi(v1 + (v1 >> 1), -32767, 32767);   /* (its centre a little louder: the band sings) */
        }
        v->b1 = b1;
        v->b2 = b2;
    }
    {                                                    /* (JIANT 0.5) the edge taken off: a one-pole low-pass at
                                                          * ~8 kHz on the sound (FM, FOLD and bright noise at 16 kHz
                                                          * were knife-sharp, the dice's above all) */
        int32_t lp = v->lp;
        for (i = 0; i < n; i++) {
            lp += ((int64_t)(y[i] - lp) * 29400) >> 15;           /* (~8 kHz at 22 kHz; 22300 at 44.1) */
            y[i] = lp;
        }
        v->lp = lp;
    }
    for (i = n; i-- > 0;) {                              /* (0.6.17) back to full rate: each half-rate sample and the
                                                          * mean of it and the one before (the last block's: v->hl) */
        int32_t a = y[i], b = i ? y[i - 1] : v->hl;
        y[2u * i + 1u] = a;
        y[2u * i] = (a + b) >> 1;
    }
    if (nf & 1u)                                         /* (an odd block: its last sample held) */
        y[nf - 1u] = y[nf - 2u];
    v->hl = y[nf - 1u];
    v->ea = ea1;
    v->en = en1;
    v->ep = ep1;
    v->inc = inc1;
    if (v->ea < (1 << 19) && v->en < (1 << 19)) {       /* -66 dB (0.6.14: was -84: a tail computed below hearing) */
        v->active = 0;
        v->hl = 0;
    }
}


