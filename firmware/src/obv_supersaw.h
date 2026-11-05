/* SPDX-License-Identifier: GPL-3.0-only */
/* SS2: seven saws inside one oscillator. Offsets and detune curve adapted
 * from isod89/clipper-fm1 eng_hybrid.c (3dSam / Leo Kuroshita, 2026),
 * itself referencing Adam Szabo's JP-8000 analysis. Independent MIX and
 * peak-normalized gains are SMKSYNTH additions; this is not a JP emulator.
 * Uses our event-corrected saw, including its one-sample delay. */
typedef struct {
    osc_t side[6];
    int32_t det_sm, mix_sm;               /* Q8 controls; slew only while SUPER */
    uint8_t active;
    uint32_t inc_cached, det_cached, si[6];
    int32_t mix_cached, cg;
} super_state_t;

static inline __attribute__((always_inline)) uint32_t super_curve(uint32_t shape)
{
    int32_t x = (int32_t)shape * 516;
    return (uint32_t)((((int64_t)x * x >> 16) * 3 + x) >> 2);
}

/* The target's -Os emits actual divides even for /16 and /6. Preserve C's
 * truncation towards zero with shifts / reciprocal multiplication instead. */
static inline __attribute__((always_inline)) int32_t super_div16(int32_t x)
{
    return x < 0 ? -((-x) >> 4) : x >> 4;
}
static inline __attribute__((always_inline)) int32_t super_div6(int32_t x)
{
    return (int32_t)(((int64_t)x * 715827883) >> 32) + (x < 0);
}

/* Specialize osc_step for a natural saw wrap. Phase and delayed sample stay
 * in registers for the whole block; H=-65536 makes the event gains exact
 * +/-1, so there are no extra multiplies/shifts. Bit-identical to osc_step. */
static inline __attribute__((always_inline)) void super_saw_run(osc_t *o, uint32_t inc,
                                                               int32_t *out, uint32_t n, int add)
{
    uint32_t ph = o->ph, d = inc >> 15;
    int32_t prev = o->prev;
    for (uint32_t i = 0; i < n; i++) {
        uint32_t old = ph; ph += inc;
        int32_t a = prev, cur = (int32_t)(ph >> 16) - 32768;
        if (ph < old && d) {
            int32_t t = (int32_t)(ph / d), u = 32767 - t;
            a -= (t * t) >> 15;
            cur += (u * u) >> 15;
        }
        if (add) out[i] += a; else out[i] = a;
        prev = cur;
    }
    o->ph = ph; o->prev = prev;
}

/* An inaudible oscillator still advances to exactly the same state as a full
 * render. Only the last naive sample and its wrap correction affect prev. */
static inline __attribute__((always_inline)) void super_saw_skip(osc_t *o, uint32_t inc, uint32_t n)
{
    uint32_t ph = o->ph + inc * n, d = inc >> 15;
    int32_t cur = (int32_t)(ph >> 16) - 32768;
    if (ph < inc && d) {
        int32_t u = 32767 - (int32_t)(ph / d);
        cur += (u * u) >> 15;
    }
    o->ph = ph; o->prev = cur;
}

#ifdef OBV_HOST
#define SUPER_HOT
#else
/* Keep the hot loop out of the flash instruction cache. No out-of-RAM calls;
 * tools/build.py audits this together with the existing RAM routines. */
#define SUPER_HOT __attribute__((section(".ram_text.super"), noinline))
#endif
static SUPER_HOT void super_render(super_state_t *s, osc_t *center, uint32_t inc,
                         uint32_t det, uint32_t mix, int32_t *out, uint32_t n)
{
    /* Mutable storage deliberately lives in RAM even on the target. */
    static int32_t off[6] __attribute__((section(".data.super"))) = {-7210, -4121, -1279, 1305, 4074, 7042};
    uint32_t j, i;
    if (!s->active) {
        uint32_t seed = center->ph ^ 0x9E3779B9u;
        for (j = 0; j < 6; j++) {
            seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
            s->side[j].ph = seed;
            s->side[j].prev = osc_naive(seed, 0, OSC_SAW);
        }
        s->det_sm = (int32_t)det * 256;
        s->mix_sm = (int32_t)mix * 256;
        s->active = 1;
        s->inc_cached = 0; s->det_cached = 128; s->mix_cached = -1;
    }
    s->det_sm += super_div16((int32_t)det * 256 - s->det_sm);
    s->mix_sm += super_div16((int32_t)mix * 256 - s->mix_sm);
    uint32_t shape = (uint32_t)s->det_sm >> 8;
    /* Keep every side below Nyquist even at extreme MIDI pitches. */
    if (inc > 0x70000000u) inc = 0x70000000u;
    if (inc != s->inc_cached || shape != s->det_cached) {
        uint32_t d = super_curve(shape);
        for (j = 0; j < 6; j++)
            s->si[j] = (uint32_t)((int64_t)inc + (((int64_t)inc * off[j] * d) >> 32));
        s->inc_cached = inc; s->det_cached = shape;
    }
    /* Central + six sides at up to half its level, peak normalized.
     * Compute the side average first: every multiply fits in int32. */
    uint32_t m = (uint32_t)s->mix_sm;
    if ((int32_t)m != s->mix_cached) {
        s->cg = (int32_t)((32768u * 32512u) / (32512u + 3u * m));
        s->mix_cached = (int32_t)m;
    }
    int32_t cg = s->cg;
    int32_t sg = 32768 - cg;
    if (!out) {
        super_saw_skip(center, inc, n);
        for (j = 0; j < 6; j++) super_saw_skip(&s->side[j], s->si[j], n);
        return;
    }
    super_saw_run(center, inc, out, n, 0);
    if (!sg) {
        for (j = 0; j < 6; j++) super_saw_skip(&s->side[j], s->si[j], n);
        return;
    }
    int32_t side[CTL];
    for (i = 0; i < n; i++) side[i] = 0;
    for (j = 0; j < 6; j++) super_saw_run(&s->side[j], s->si[j], side, n, 1);
    for (i = 0; i < n; i++) out[i] = (out[i] * cg + super_div6(side[i]) * sg) >> 15;
}
#undef SUPER_HOT

static inline void super_render_call(super_state_t *s, osc_t *center, uint32_t inc,
                                     uint32_t det, uint32_t mix, int32_t *out, uint32_t n)
{
#ifdef OBV_HOST
    super_render(s, center, inc, det, mix, out, n);
#else
    /* Same far-call mechanism as the existing flash RAM routines: XIP->RAM
     * exceeds the target's relative call range. No interrupt masking. */
    __typeof__(&super_render) volatile fn = &super_render;
    fn(s, center, inc, det, mix, out, n);
#endif
}
