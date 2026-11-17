/* SPDX-License-Identifier: GPL-3.0-only */
/* DC4: same full-rate FX/master state and equations. Main/USB stays -Os;
 * the separate pure DSP object is -O2. No extra buffering or core transfer. */
#include "obv_fx_shared.h"
static int16_t fx_mem[FX_MEM_N] __attribute__((section(".pool")));
static uint32_t fx_used;
line_t cho, dl, dr, rv_pre, rv_in[4], rv_ap1, rv_d1, rv_ap2, rv_d2, rv_ap3, rv_d3, rv_ap4, rv_d4;
uint32_t cho_ph, cho_inc, rv_ph1, rv_ph2;
int32_t cho_depth_k, cho_mix, dly_mix, rev_mix, dly_fb, dly_tone_k, rev_decay, rev_damp_k;
int32_t dly_time, dly_cur;               /* Q16 samples: the target and where the read is (it slews like a tape) */
int32_t dly_lpl, dly_lpr, rv_lp_in, rv_lpl, rv_lpr, rv_yl, rv_yr;
#if REV_DIV == 2u
int32_t rv_hold;
#endif
uint32_t rev_pre_n = 1, dly_active, rev_active;
#if REV_DIV == 2u
uint32_t fx_odd;
#endif

volatile uint8_t ctl_dv_type, ctl_dv_amt = 60, ctl_dv_tone = 90, ctl_dv_mix = 127;
int32_t dv_lp, dv_hold;                  /* Q15 */
uint32_t dv_n;
volatile uint8_t ctl_m_vol = 100, ctl_m_dj = 64, ctl_m_drive, ctl_m_comp;   /* (main loop writes) volume (100 =
                                                 * unity), DJ filter (64 off, below LP, above HP), drive, compressor */
volatile uint32_t ui_out_peak;           /* the output's peak, for the MASTER row's meter (the UI takes it) */
svf_coef_t m_c;
svf_state_t m_fl, m_fr;
int32_t m_dj_was = 0x7FFF, m_env, m_g = 1 << 14;   /* m_env: the level follower Q23; m_g: the gain now, Q14 */

static void line_init(line_t *l, uint32_t len) { l->b = fx_mem + fx_used; l->len = len; l->w = 0; fx_used += len; }
static void fx_init(void)
{
    line_init(&cho, CHO_N);
    line_init(&dl, DLY_MAX);
    line_init(&dr, DLY_MAX);
    line_init(&rv_pre, REV_PRE_N);
    line_init(&rv_in[0], RV_IN1); line_init(&rv_in[1], RV_IN2); line_init(&rv_in[2], RV_IN3); line_init(&rv_in[3], RV_IN4);
    line_init(&rv_ap1, RV_AP1 + 16u); line_init(&rv_d1, RV_D1); line_init(&rv_ap2, RV_AP2); line_init(&rv_d2, RV_D2);
    line_init(&rv_ap3, RV_AP3 + 16u); line_init(&rv_d3, RV_D3); line_init(&rv_ap4, RV_AP4); line_init(&rv_d4, RV_D4);
    dly_cur = dly_time = 11025 << 16;
}

/* the patch's numbers, once per ISR call. quarter: frames per quarter note (the arp's clock) */
static void fx_block(const patch_t *p, uint32_t quarter)
{
    int32_t d, t;
    uint32_t div = p->v[P_DLY_TIME] % NDDIV, samples;
    d = p->v[P_CHO_RATE]; cho_inc = (uint32_t)(10 + ((d * d * 590) >> 14)) * 974u;   /* 0.1 .. 5.9 Hz */
    cho_depth_k = p->v[P_CHO_DEPTH] * 67;       /* x the triangle's +-2048 = Q16 samples: up to +-6 ms */
    cho_mix = p->v[P_CHO_MIX];
    dly_mix = (p->v[P_DLY_MIX] * 180) >> 7;     /* wet up to 0.7 of the dry (Q8): the sum stays in range */
    rev_mix = (p->v[P_REV_MIX] * 180) >> 7;
    dly_fb = p->v[P_DLY_FB] * 250;              /* up to 0.97 */
    t = p->v[P_DLY_TONE]; dly_tone_k = clampi(((t * t) >> 6) + 8, 8, 256);
    d = p->v[P_REV_SIZE]; rev_decay = 12000 + ((d * d * 19500) >> 14);   /* 0.37 .. 0.96: the top is a near-freeze */
    d = p->v[P_REV_DAMP]; rev_damp_k = 32768 - d * d * 2;   /* the one-pole's k at the half rate */
    if (REV_DIV == 1u) {                        /* the same corner at the full rate: k' = 1 - sqrt(1 - k) */
        uint32_t x = (uint32_t)(32768 - rev_damp_k) << 15, r = 0, bit = 1u << 15;
        for (; bit; bit >>= 1)
            if ((r + bit) * (r + bit) <= x) r += bit;
        rev_damp_k = 32768 - (int32_t)r;
    }
    rev_pre_n = 1u + (uint32_t)p->v[P_REV_PRE] * (41u / REV_DIV);
    samples = quarter * DDIV_NUM[div] / DDIV_DEN[div];
    if (samples > DLY_MAX - 4u) samples = DLY_MAX - 4u;
    if (samples < 64u) samples = 64u;
    dly_time = (int32_t)(samples << 16);
    if (dly_mix) dly_active = 12u * FS; else if (dly_active > HALF_FRAMES) dly_active -= HALF_FRAMES; else dly_active = 0;
    if (rev_mix) rev_active = 24u * FS; else if (rev_active > HALF_FRAMES) rev_active -= HALF_FRAMES; else rev_active = 0;
}


#ifndef OBV_FX_EXTERNAL
#ifdef OBV_HOST
#define OBV_FX_EXTERNAL 0
#else
#define OBV_FX_EXTERNAL 1
#endif
#endif
#if OBV_FX_EXTERNAL
void fx_process(int32_t*,const int32_t*,const int32_t*,const int32_t*,uint32_t);
void drive_process(int32_t*,const int32_t*,uint32_t);
void master_process(int32_t*,uint32_t);
#else
#define OBV_FX_LINKAGE static
#include "obv_fx_process.h"
#undef OBV_FX_LINKAGE
#endif
