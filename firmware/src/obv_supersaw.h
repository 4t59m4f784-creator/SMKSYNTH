/* SPDX-License-Identifier: GPL-3.0-only */
/* SS1: seven saws inside one oscillator. Offsets and detune curve adapted
 * from isod89/clipper-fm1 eng_hybrid.c (3dSam / Leo Kuroshita, 2026),
 * itself referencing Adam Szabo's JP-8000 analysis. Independent MIX and
 * peak-normalized gains are SMKSYNTH additions; this is not a JP emulator.
 * Uses our event-corrected saw, including its one-sample delay. */
typedef struct {
    osc_t side[6];
    int32_t det_sm, mix_sm;               /* Q8 controls; slew only while SUPER */
    uint8_t active;
} super_state_t;

static uint32_t super_curve(uint32_t shape)
{
    int32_t x = (int32_t)shape * 516;
    return (uint32_t)((((int64_t)x * x >> 16) * 3 + x) >> 2);
}

static void super_render(super_state_t *s, osc_t *center, uint32_t inc,
                         uint32_t det, uint32_t mix, int32_t *out, uint32_t n)
{
    static const int32_t off[6] = {-7210, -4121, -1279, 1305, 4074, 7042};
    uint32_t si[6], j, i;
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
    }
    s->det_sm += ((int32_t)det * 256 - s->det_sm) / 16;
    s->mix_sm += ((int32_t)mix * 256 - s->mix_sm) / 16;
    uint32_t d = super_curve((uint32_t)s->det_sm >> 8);
    /* Keep every side below Nyquist even at extreme MIDI pitches. */
    if (inc > 0x70000000u) inc = 0x70000000u;
    for (j = 0; j < 6; j++)
        si[j] = (uint32_t)((int64_t)inc + (((int64_t)inc * off[j] * d) >> 32));
    /* Central + six sides at up to half its level, peak normalized.
     * Compute the side average first: every multiply fits in int32. */
    uint32_t m = (uint32_t)s->mix_sm;
    int32_t cg = (int32_t)((32768u * 32512u) / (32512u + 3u * m));
    int32_t sg = 32768 - cg;
    for (i = 0; i < n; i++) {
        int32_t a = osc_step(center, inc, inc >> 15, 0, OSC_SAW, -65536, -1);
        int32_t sum = 0;
        for (j = 0; j < 6; j++)
            sum += osc_step(&s->side[j], si[j], si[j] >> 15, 0, OSC_SAW, -65536, -1);
        out[i] = (a * cg + (sum / 6) * sg) >> 15;
    }
}
