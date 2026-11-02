/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
 * Portions Copyright (C) 2026 Juan Ignacio Antonucci (Juan Jiant)
 * Independent original render body; WORDS call excluded, symbols adapted. */
static int32_t ref_noise=0x2545F491;
static uint32_t ref_noise32(int32_t *s){uint32_t x=(uint32_t)*s;x^=x<<13;x^=x>>17;x^=x<<5;*s=(int32_t)x;return x;}
static int32_t ref_amp(int32_t x,const jv_mod_t*m,uint32_t i){int32_t d=(m->amp1-m->amp0)*(int32_t)i;int32_t a=m->amp0+((d+((d>>31)&31))>>5);return ((x*a)>>15)*24000>>15;}
static int32_t ref_knee(int32_t y,int32_t k){int32_t a=y<0?-y:y;if(a<=k)return y;a=k+(softclip((a-k)*2)>>1);return y<0?-a:a;}
static int32_t ref_glide(const jv_voice_t *v)
{
    int32_t d = v->pitch16 - v->gfrom, u;
    if (!d || v->pitch_cur == v->pitch16)
        return v->pitch_cur << 8;
    u = ((((v->pitch_cur - v->gfrom) << 8) + (d > 0 ? v->gacc : -v->gacc)) << 4) / d;   /* Q12 (|d| < 2048) */
    if ((uint32_t)u > 4096u)                            /* (not this glide's: as it is) */
        return v->pitch_cur << 8;
    u = (((u * u) >> 12) * (3 * 4096 - 2 * u)) >> 12;
    return (v->gfrom << 8) + ((d * u) >> 4);
}
/* the phase increment of the pitch as sung: m's (voice.c: the glide's linear course and the rest) moved by the S-curve's
 * offset from that course; *p16 the pitch (1/16 st). Only while the voice glides (the renders' block header) */
static __attribute__((noinline)) uint32_t ref_sung(const jv_voice_t *v, const jv_mod_t *m, int32_t *p16)
{
    int32_t g = ref_glide(v) - (v->pitch_cur << 8), fine = m->fine + (((g & 255) * 237) >> 12);   /* (1/16 st: 14.8 / 4096) */
    uint32_t inc;
    *p16 = clampi(m->pitch16 + (g >> 8), 0, 2047);
    inc = jv_pitch_inc(*p16);
    return fine ? inc + (uint32_t)((int32_t)(inc >> 12) * fine) : inc;
}

static void ref_render(jv_track_t *t, jv_voice_t *v, int32_t *out, uint32_t n, const jv_mod_t *m)
{
    const int16_t *p = t->p;
    fres_t r1, r2, r3, r4;
    int32_t fa[3], ba[3], fb[3], bb[3], k, j, buzz, oq, tilt, gv, gn, og, ogk;
    uint32_t inc = m->inc, i, te, tp, rp, rn, ratio, bws, f0;
    uint32_t ph = v->ph[0];
    int32_t y1 = v->s[0], y2 = v->s[1], y3 = v->s[2], y4 = v->s[3], y5 = v->s[4], y6 = v->s[5];
    int32_t y7 = (int32_t)v->ph[1], y8 = (int32_t)v->ph[2], lp = v->s[6], nst = ref_noise;

    if (v->pitch_cur != v->pitch16) {                   /* (0.9.8) gliding: as sung, an S-curve */
        int32_t p16;
        inc = ref_sung(v, m, &p16);
    }
    /* vowel: VOWEL -> VOWL2 over TALK after note-on; ENV / LFO -> FLT move both */
    k = 0;
    if (p[P_E2]) {
        uint32_t g = ((uint32_t)v->s[7] & JV_TALK_MASK) + JV_ENV_LIN[p[P_E2] & 127];
        g = g > (1u << 24) ? 1u << 24 : g;
        v->s[7] = (int32_t)(((uint32_t)v->s[7] & ~(uint32_t)JV_TALK_MASK) | g);
        k = (int32_t)(g >> 9);                          /* Q15 */
        k = (k * (65536 - k)) >> 15;                    /* ease out: the mouth opens fast, then settles */
    }
    {   /* RAND: both vowels move towards this note's random vowel (0 = off, 127 = fully random) */
        int32_t rv = (int32_t)(((uint32_t)v->s[7] >> 25) << 8), v0 = p[P_E0] << 8, v1 = p[P_E1] << 8;
        v0 += ((rv - v0) * p[P_E7] * 258) >> 15;
        v1 += ((rv - v1) * p[P_E7] * 258) >> 15;
        vowel_at(v0 + m->cutoff, fa, ba);
        vowel_at(v1 + m->cutoff, fb, bb);
    }
    /* SHIFT (semitones) and FOLOW (key tracking around C4, fixed), one ratio for all four */
    ratio = fmt_ratio(p[P_E3] * 16 + ((FMT_FOLOW * (v->pitch_cur - 60 * 16) * 516) >> 16));   /* FOLOW / 127 */
    bws = fmt_ratio((64 - p[P_E6]) * 4);                /* Q: bandwidth x2.5 .. x0.4 */
    f0 = (uint32_t)(((uint64_t)inc * 705600u) >> 32);   /* F0 in Hz * 16 */
    for (j = 0; j < 3; j++) {
        int32_t f = fa[j] + (((fb[j] - fa[j]) * k) >> 15), b = ba[j] + (((bb[j] - ba[j]) * k) >> 15);
        f = (int32_t)(((int64_t)f * ratio) >> 16);
        b = (int32_t)(((((int64_t)b * ratio) >> 16) * bws) >> 16);
        if (j == 0 && (uint32_t)f < f0 + (f0 >> 4) && f > 0) {   /* sung high: F1 follows F0 (sopranos do), */
            b = (int32_t)(((int64_t)b * (int32_t)(f0 + (f0 >> 4))) / f);   /* same Q: no boom on the fundamental */
            f = (int32_t)(f0 + (f0 >> 4));
        }
        fa[j] = clampi(f, 60 * 16, 7000 * 16);
        ba[j] = clampi(b, 20 * 16, 1500 * 16);
    }
    fres_coef(&r1, (uint32_t)fa[0], (uint32_t)ba[0]);
    fres_coef(&r2, (uint32_t)fa[1], (uint32_t)ba[1]);
    fres_coef(&r3, (uint32_t)fa[2], (uint32_t)ba[2]);
    fres_coef(&r4, (uint32_t)clampi((int32_t)((3300u * 16u * (uint64_t)ratio) >> 16), 60 * 16, 7000 * 16),
              (uint32_t)clampi((int32_t)((((250u * 16u * (uint64_t)ratio) >> 16) * bws) >> 16), 20 * 16, 1500 * 16));

    /* source: BUZZ (+ SHP modulation, + velocity) -> open quotient and tilt */
    buzz = clampi((p[P_E4] << 8) + m->shape - (64 << 8) + (v->vel - 96) * 40, 0, 127 << 8);
    oq = 58982 - ((buzz * 284) >> 8);                   /* Q16: 0.90 .. 0.35 */
    tilt = 2500 + ((buzz * 109) >> 8);                  /* one-pole coefficient Q14: dark .. open */
    te = (uint32_t)oq << 16;                            /* closure (end of the open phase) */
    tp = (te >> 8) * 166u;                              /* opening: 65 % of it, closing 35 % */
    rp = (1u << 30) / ((tp >> 16) | 1u);
    rn = (1u << 30) / (((te - tp) >> 16) | 1u);
    gv = 32767 - p[P_E5] * p[P_E5] * 2;                 /* BREATH: voicing fades at the top */
    gn = p[P_E5] * 258;                                 /* noise: Q15, up to 1.0 */
    og = 13000 - ((p[P_E6] - 64) * 50);                 /* output: narrower bands ring louder */
    og = (int32_t)(((int64_t)og * fmt_ratio((60 * 16 - v->pitch_cur) / 4)) >> 16);   /* 1.5 dB / oct: low notes are sparse pulses */
    ogk = (og >> 2) * (4 + (buzz >> 10));               /* presence k = 1 .. 4.9 */

    for (i = 0; i < n; i++) {
        int32_t e, x, a, s;
        if (ph < tp)                                    /* opening: + sin(pi u) * Tn / Tp */
            e = (fmt_sine(((ph >> 16) * rp) << 1) * 17644) >> 15;
        else if (ph < te)                               /* closing: - sin(pi / 2 u) */
            e = -fmt_sine(((ph - tp) >> 16) * rn);
        else
            e = 0;
        e += fmt_blep(ph - te, inc) >> 1;                   /* the closure step, band-limited */
        lp += ((e - lp) * tilt) >> 14;
        x = (lp * gv) >> 15;
        if (gn) {
            int32_t nz = (((int32_t)(ref_noise32(&nst) >> 16) - 32768) * gn) >> 15;
            x += ph < te ? nz : nz >> 1;                /* aspiration follows the glottal opening */
        }
        /* F1 -> F2 -> F3 -> F4, Q30, rounded; the signal x 64 inside: the rounding
         * dead band of F1 (+-0.5 / A, ~340 at 270 Hz) stays below 6 LSB */
        a = (int32_t)(((int64_t)r1.a * (x << 6) + (int64_t)r1.b * y1 + (int64_t)r1.c * y2 + (1 << 29)) >> 30);
        y2 = y1;
        y1 = a;
        a = (int32_t)(((int64_t)r2.a * a + (int64_t)r2.b * y3 + (int64_t)r2.c * y4 + (1 << 29)) >> 30);
        y4 = y3;
        y3 = a;
        a = (int32_t)(((int64_t)r3.a * a + (int64_t)r3.b * y5 + (int64_t)r3.c * y6 + (1 << 29)) >> 30);
        y6 = y5;
        y5 = a;
        a = (int32_t)(((int64_t)r4.a * a + (int64_t)r4.b * y7 + (int64_t)r4.c * y8 + (1 << 29)) >> 30);
        y8 = y7;
        y7 = clampi(a, -(1 << 28), 1 << 28);             /* int32 state stays far from overflow */
        s = ref_knee(clampi((int32_t)(((int64_t)y7 * og + (int64_t)(y7 - y8) * ogk) >> 21), -200000, 200000), 24000);
        ph += inc;
        out[i] += ref_amp(s, m, i) << 1;
    }
    v->ph[0] = ph;
    v->ph[1] = (uint32_t)y7;
    v->ph[2] = (uint32_t)y8;
    ref_noise = nst;
    v->s[0] = y1;
    v->s[1] = y2;
    v->s[2] = y3;
    v->s[3] = y4;
    v->s[4] = y5;
    v->s[5] = y6;
    v->s[6] = lp;
}

