/* SPDX-License-Identifier: GPL-3.0-only */
#pragma once
#ifndef REV_DIV
#define REV_DIV 1u
#endif
#define RVL(x) ((uint32_t)((x) * 44100u / 29761u / REV_DIV))   /* the paper's 29761 Hz lengths at our rate */
#define DLY_MAX 32768u                          /* a side of the delay: 743 ms */
#define CHO_N 1024u
#define REV_PRE_N (8192u / REV_DIV)             /* up to 120 ms of pre-delay */
#define RV_IN1 RVL(142)
#define RV_IN2 RVL(107)
#define RV_IN3 RVL(379)
#define RV_IN4 RVL(277)
#define RV_AP1 RVL(672)
#define RV_D1 RVL(4453)
#define RV_AP2 RVL(1800)
#define RV_D2 RVL(3720)
#define RV_AP3 RVL(908)
#define RV_D3 RVL(4217)
#define RV_AP4 RVL(2656)
#define RV_D4 RVL(3163)
#define REV_N (REV_PRE_N + RV_IN1 + RV_IN2 + RV_IN3 + RV_IN4 + RV_AP1 + 16u + RV_D1 + RV_AP2 + RV_D2 + RV_AP3 + 16u + RV_D3 + RV_AP4 + RV_D4)
#define RV_BW (REV_DIV == 2u ? 24000 : 16700)   /* the input's one-pole: ~5 kHz either way */
#define FX_MEM_N (CHO_N + 2u * DLY_MAX + REV_N)

typedef struct { int16_t *b; uint32_t len, w; } line_t;   /* w: where the next sample goes */
static inline int32_t sat16(int32_t v) { return v > 32767 ? 32767 : v < -32767 ? -32767 : v; }
static inline __attribute__((always_inline)) int32_t line_read(const line_t *l, uint32_t back)   /* 1 <= back <= len */
{
    uint32_t i = l->w + l->len - back;
    if (i >= l->len) i -= l->len;
    return l->b[i];
}
static inline __attribute__((always_inline)) void line_write(line_t *l, int32_t v)
{
    l->b[l->w] = (int16_t)sat16(v);
    if (++l->w >= l->len) l->w = 0;
}
/* Dattorro's allpass: the line holds the inner node, the output is tapped from it. g Q15, signed */
static inline __attribute__((always_inline)) int32_t ap_step(line_t *l, int32_t in, int32_t g)
{
    int32_t d = line_read(l, l->len), v = in - ((d * g) >> 15);
    line_write(l, v);
    return d + ((v * g) >> 15);
}
/* the same with the delay swaying by e (Q8 samples, |e| <= 8 << 8) around base; the line is base + 16 long */
static inline __attribute__((always_inline)) int32_t ap_mod(line_t *l, int32_t in, int32_t g, uint32_t base, int32_t e)
{
    uint32_t back = (uint32_t)((int32_t)base + (e >> 8));
    int32_t f = e & 255, a = line_read(l, back), b = line_read(l, back + 1u), d = a + (((b - a) * f) >> 8), v;
    v = in - ((d * g) >> 15);
    line_write(l, v);
    return d + ((v * g) >> 15);
}
static inline int32_t tri_q(uint32_t ph)        /* a triangle -2048..2047 (so Q8 = +-8 samples) */
{
    int32_t t = (int32_t)(ph >> 19);            /* 0..8191 */
    return t < 4096 ? t - 2048 : 6143 - t;
}

/* DC4: these taps belong only to fixed power-of-two delay/chorus lines.
 * Unsigned masking is exactly the old single-wrap result for valid taps;
 * it avoids reloading len and branching for every interpolation point. */
_Static_assert((DLY_MAX&(DLY_MAX-1u))==0u,"delay length must be power of two");
_Static_assert((CHO_N&(CHO_N-1u))==0u,"chorus length must be power of two");
static inline __attribute__((always_inline)) int32_t dly_read_q16(const line_t *l,int32_t back_q16)
{
    uint32_t b=(uint32_t)back_q16>>16, i=(l->w-b)&(DLY_MAX-1u);
    int32_t f=(back_q16>>8)&255, a=l->b[i], c=l->b[(i-1u)&(DLY_MAX-1u)];
    return a+(((c-a)*f)>>8);
}
static inline __attribute__((always_inline)) int32_t cho_read_q16_h(const line_t *l,int32_t back_q16)
{
    uint32_t b=(uint32_t)back_q16>>16, i=(l->w-b)&(CHO_N-1u);
    int32_t f=(back_q16>>8)&255;
    int32_t p0=l->b[(i+1u)&(CHO_N-1u)],p1=l->b[i],p2=l->b[(i-1u)&(CHO_N-1u)],p3=l->b[(i-2u)&(CHO_N-1u)];
    int32_t c1=(p2-p0)>>1,c2=p0-((5*p1)>>1)+2*p2-(p3>>1),c3=((p3-p0)>>1)+((3*(p1-p2))>>1);
    return p1+((f*(c1+((f*(c2+((f*c3)>>8)))>>8)))>>8);
}

extern line_t cho, dl, dr, rv_pre, rv_in[4], rv_ap1, rv_d1, rv_ap2, rv_d2, rv_ap3, rv_d3, rv_ap4, rv_d4;
extern uint32_t cho_ph, cho_inc, rv_ph1, rv_ph2;
extern int32_t cho_depth_k, cho_mix, dly_mix, rev_mix, dly_fb, dly_tone_k, rev_decay, rev_damp_k;
extern int32_t dly_time, dly_cur;               /* Q16 samples: the target and where the read is (it slews like a tape) */
extern int32_t dly_lpl, dly_lpr, rv_lp_in, rv_lpl, rv_lpr, rv_yl, rv_yr;
#if REV_DIV == 2u
extern int32_t rv_hold;
#endif
extern uint32_t rev_pre_n, dly_active, rev_active;
#if REV_DIV == 2u
extern uint32_t fx_odd;
#endif

enum { DV_OVERDRIVE, DV_DIST, DV_FUZZ, DV_CRUSH, NDV };
extern volatile uint8_t ctl_dv_type, ctl_dv_amt, ctl_dv_tone, ctl_dv_mix;
extern int32_t dv_lp, dv_hold;                  /* Q15 */
extern uint32_t dv_n;
extern volatile uint8_t ctl_m_vol, ctl_m_dj, ctl_m_drive, ctl_m_comp;   /* (main loop writes) volume (100 =
                                                 * unity), DJ filter (64 off, below LP, above HP), drive, compressor */
extern volatile uint32_t ui_out_peak;           /* the output's peak, for the MASTER row's meter (the UI takes it) */
extern svf_coef_t m_c;
extern svf_state_t m_fl, m_fr;
extern int32_t m_dj_was, m_env, m_g;   /* m_env: the level follower Q23; m_g: the gain now, Q14 */

