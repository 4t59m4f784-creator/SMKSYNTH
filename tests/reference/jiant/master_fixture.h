/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
 * Original default fx.c blocks, renamed; optional LOWCUT branch omitted. */
static int32_t ref_mix_l[32],ref_mix_r[32],ref_wet[32],ref_lim_env=18000,ref_dc_l,ref_dc_r,ref_dce_l,ref_dce_r;
#define ref_LIM_T 18000
#define ref_LEV_T 12000                                     /* (the mix's scale: Q15 at MASTER full) ~ -9 dBFS peaks: the
                                                         * limiter's headroom (0.5.1: was 16000, it limited 95 % of the time) */
#define ref_LEV_GATE 400                                    /* -38 dB */
static int32_t ref_lev_env, ref_lev_g = 32768, ref_lev_cur = 32768, ref_lev_dg;   /* the gain now and its step a sample (ref_master_out) */
static __attribute__((noinline)) void ref_lev_block(uint32_t n)
{
    uint32_t i;
    int32_t pk = 0, g1;
    for (i = 0; i < n; i++) {
        int32_t l = ref_mix_l[i] + ref_wet[i], r = ref_mix_r[i] + ref_wet[i];
        l = l < 0 ? -l : l;
        r = r < 0 ? -r : r;
        if (l > pk) pk = l;
        if (r > pk) pk = r;
    }
    ref_lev_env += pk > ref_lev_env ? (pk - ref_lev_env) >> 6 : -((ref_lev_env - pk) >> 11);   /* (0.5.1: ~45 ms up, ~1.5 s down: it
                                                         * rides the song's loudness, not each hit; was ~1 / ~46 ms) */
    g1 = 32768;                                         /* (below the gate: back to unity, slowly) */
    if (ref_lev_env > ref_LEV_GATE && pk > ref_LEV_GATE) {         /* (0.5.1: the block itself above the gate too: a tail
                                                         * dying under it is not lifted while the slow follower falls) */
        uint32_t q = ((uint32_t)ref_LEV_T << 12) / (uint32_t)ref_lev_env, rt = 0, b = 1u << 30;   /* ref_LEV_T / level, Q12 */
        q <<= 8;                                        /* (Q20: its square root Q10) */
        while (b > q) b >>= 2;
        while (b) {
            if (q >= rt + b) { q -= rt + b; rt = (rt >> 1) + b; }
            else rt >>= 1;
            b >>= 2;
        }
        g1 = clampi((int32_t)rt << 5, 11500, 65536);     /* Q15: x0.35 .. x2 */
    }
    g1 = ref_lev_g + ((g1 - ref_lev_g) >> (ref_lev_env > ref_LEV_GATE && pk > ref_LEV_GATE ? 8 : 5));   /* (smoothed: ~190 ms; gated, under -38 dB: ~25 ms
                                                         * back to unity, no gain left on a tail's offset) */
    ref_lev_cur = ref_lev_g;                                    /* (ref_master_out ramps it across the block, after the DC block) */
    ref_lev_dg = (g1 - ref_lev_g) / (int32_t)n;
    ref_lev_g = g1;
}
static inline int32_t ref_dc_block(int32_t x, int32_t *dc, int32_t *err)
{
    int32_t e = (x << 6) - *dc + *err, d = e >> 12;
    *err = e - (d << 12);
    *dc += d;
    return x - ((*dc + 32) >> 6);
}

static inline void ref_master_out(int32_t *l, int32_t *r)
{
    int32_t al, ar, a;
    *l = ref_dc_block(*l, &ref_dc_l, &ref_dce_l);
    *r = ref_dc_block(*r, &ref_dc_r, &ref_dce_r);
    *l = (clampi(*l, -262143, 262143) * (ref_lev_cur >> 4)) >> 11;   /* (JIANT 0.5) the leveler's gain (ref_lev_block) */
    *r = (clampi(*r, -262143, 262143) * (ref_lev_cur >> 4)) >> 11;
    ref_lev_cur += ref_lev_dg;
    al = *l < 0 ? -*l : *l;
    ar = *r < 0 ? -*r : *r;
    a = al > ar ? al : ar;
    if (a > ref_lim_env)
        ref_lim_env += (a - ref_lim_env) >> 2;
    else if (ref_lim_env > ref_LIM_T)
        ref_lim_env -= ((ref_lim_env - ref_LIM_T) >> 11) + 1;   /* (0.5.1: ~45 ms back, was ~90) */
    if (ref_lim_env > ref_LIM_T) {
        int32_t g = (int32_t)(((uint32_t)ref_LIM_T << 15) / (uint32_t)ref_lim_env);   /* < 32768 */
        *l = ((*l >> 4) * g) >> 11;                      /* >> 4 first: |l| may be far above Q15 */
        *r = ((*r >> 4) * g) >> 11;
    }
    *l = softclip(*l);
    *r = softclip(*r);
}

