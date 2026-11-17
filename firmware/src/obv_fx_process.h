/* SPDX-License-Identifier: GPL-3.0-only */
/* Identical DC3 equations, compiled independently of USB/UI. */
/* one reverb sample at the half rate: in -> pre-delay -> bandwidth -> 4 diffusers -> the two tank halves */
static inline void rev_tick(int32_t in)
{
    int32_t x, a, b, e1 = tri_q(rv_ph1), e2 = tri_q(rv_ph2), dec = rev_decay, dk = rev_damp_k;
    rv_ph1 += 97391u * REV_DIV;                 /* ~1.0 Hz and ~0.7 Hz: the tank's sway against metallic rings */
    rv_ph2 += 68174u * REV_DIV;
    line_write(&rv_pre, in);
    x = line_read(&rv_pre, rev_pre_n);
    rv_lp_in += ((x - rv_lp_in) * RV_BW) >> 15;
    x = ap_step(&rv_in[0], rv_lp_in, 24576);
    x = ap_step(&rv_in[1], x, 24576);
    x = ap_step(&rv_in[2], x, 20480);
    x = ap_step(&rv_in[3], x, 20480);
    /* left half, fed by the right's end */
    a = x + ((line_read(&rv_d4, RV_D4) * dec) >> 15);
    a = ap_mod(&rv_ap1, a, -22938, RV_AP1, e1);
    line_write(&rv_d1, a);
    b = line_read(&rv_d1, RV_D1);
    rv_lpl += ((b - rv_lpl) * dk) >> 15;
    b = ap_step(&rv_ap2, (rv_lpl * dec) >> 15, 16384);
    line_write(&rv_d2, b);
    /* right half, fed by the left's end */
    a = x + ((line_read(&rv_d2, RV_D2) * dec) >> 15);
    a = ap_mod(&rv_ap3, a, -22938, RV_AP3, e2);
    line_write(&rv_d3, a);
    b = line_read(&rv_d3, RV_D3);
    rv_lpr += ((b - rv_lpr) * dk) >> 15;
    b = ap_step(&rv_ap4, (rv_lpr * dec) >> 15, 16384);
    line_write(&rv_d4, b);
    /* the output taps (the paper's), x 0.6 */
    a = line_read(&rv_d3, RVL(266)) + line_read(&rv_d3, RVL(2974)) - line_read(&rv_ap4, RVL(1913)) + line_read(&rv_d4, RVL(1996))
      - line_read(&rv_d1, RVL(1990)) - line_read(&rv_ap2, RVL(187)) - line_read(&rv_d2, RVL(1066));
    b = line_read(&rv_d1, RVL(353)) + line_read(&rv_d1, RVL(3627)) - line_read(&rv_ap2, RVL(1228)) + line_read(&rv_d2, RVL(2673))
      - line_read(&rv_d3, RVL(2111)) - line_read(&rv_ap4, RVL(335)) - line_read(&rv_d4, RVL(121));
    rv_yl = (a * 19661) >> 15;
    rv_yr = (b * 19661) >> 15;
}

OBV_FX_LINKAGE void drive_process(int32_t *out, const int32_t *vs, uint32_t n)
{
    uint32_t i, ty = ctl_dv_type % NDV, amt = ctl_dv_amt;
    int32_t pre = 256 + (int32_t)(24u * 256u * amt / 127u);   /* Q8: 1 .. 25 */
    int32_t k = 1311 + (int32_t)(31457u * ctl_dv_tone * ctl_dv_tone / 16129u);   /* Q15: 0.04 .. 1 */
    int32_t ret = (int32_t)(ctl_dv_mix * 4000u / 127u);   /* (MIX full: the return about as loud as the dry sound) */
    if (!ctl_dv_mix) return;
    for (i = 0; i < n; i++) {
        int32_t x = (vs[i] * pre) >> 9, y;      /* softclip(x) = tanh(the drive's x), Q15 */
        if (ty == DV_OVERDRIVE) y = softclip(x);
        else if (ty == DV_DIST) y = x > 0 ? softclip(x * 8 / 5) : (softclip(x * 7 / 10) * 4) / 5;
        else if (ty == DV_FUZZ) y = clampi(x * 40, -32767, 32767);   /* (20 x the drive's x, square beyond 1/20) */
        else {                                  /* CRUSH: 16 steps, a sample held for 2..12 */
            if (!dv_n--) { dv_n = 2u + amt * 10u / 127u; dv_hold = softclip(x) * 8 / 32768 * 4096; }
            y = dv_hold;
        }
        dv_lp += ((y - dv_lp) * k) >> 15;
        {
            int32_t v = (dv_lp * ret + 16384) >> 15;   /* (rounded: the one-pole's -1 it settles on stays silent) */
            out[2u * i] = sat16(out[2u * i] + v);
            out[2u * i + 1u] = sat16(out[2u * i + 1u] + v);
        }
    }
}

static int32_t log2_q16(uint32_t x)             /* x > 0 -> log2(x) in Q16 (+-0.01) */
{
    int32_t e = 31, f;
    if (!x) return -(32 << 16);
    while (!(x >> e)) e--;
    f = (int32_t)(e >= 16 ? x >> (e - 16) : x << (16 - e)) - 65536;   /* the mantissa - 1, Q16 */
    return (e << 16) + f + (int32_t)(((int64_t)f * (65536 - f) * 22713) >> 32);   /* log2(1+f) ~ f + 0.3466 f (1-f) */
}
static int32_t exp2_q14(int32_t l)              /* 2^(l / 65536) in Q14, l within +-14 octaves */
{
    int32_t e = l >> 16, f = l & 0xFFFF, m;
    m = 16384 + ((f * 10756) >> 16) + (int32_t)(((int64_t)f * f * 5628) >> 32);   /* 2^f ~ 1 + 0.6565 f + 0.3435 f^2 */
    return e >= 0 ? (e > 14 ? 0x7FFFFFFF : m << e) : (e < -14 ? 0 : m >> -e);
}

OBV_FX_LINKAGE void master_process(int32_t *out, uint32_t n)
{
    int32_t dj = (int32_t)ctl_m_dj - 64, mode = dj <= -2 ? 1 : dj >= 2 ? 2 : 0, ad = dj < 0 ? -dj : dj;
    uint32_t d = ctl_m_drive, cm = ctl_m_comp, i, b, pk = 0;
    int32_t pre = 256 + (int32_t)(8u * 256u * d / 127u), post = (int32_t)(4096u * (127u + d) * 256u / (127u * (uint32_t)pre));
    int32_t vol = (int32_t)(ctl_m_vol * ctl_m_vol * 4096u / 10000u);   /* Q12, 100 = unity, the square law */
    if (dj != m_dj_was) {                       /* LP 18 kHz .. 60 Hz, HP 20 Hz .. 6 kHz, Q 1.1 */
        svf_coef(&m_c, mode == 1 ? 34099 - ad * 395 : 3968 + ad * 395, 3724);
        if (!mode || (m_dj_was <= -2) != (dj <= -2)) m_fl.ic1 = m_fl.ic2 = m_fr.ic1 = m_fr.ic2 = 0;
        m_dj_was = dj;
    }
    for (b = 0; b < n; b += CTL) {
        int32_t g0 = m_g, g1 = 1 << 14;
        if (cm) {                               /* the gain at this block's end: from the level so far */
            int32_t ex = (int32_t)(65536u * 5u * cm / (127u + 5u * cm));   /* Q16: the ratio's 1 - 1/r, 0 .. 0.83 */
            int32_t thr = -(int32_t)((508u + 20u * cm) * 857u / 10u);      /* log2 Q16 of -4 .. -24 dB */
            int32_t lv = log2_q16((uint32_t)(m_env >> 8) + 1u) - (15 << 16);   /* the level, log2 of full scale */
            int32_t gl = (int32_t)(((int64_t)(-thr) * ex * 29491) >> 32);  /* make-up: (1/thr)^(0.45 ex) */
            if (lv > thr) gl -= (int32_t)(((int64_t)(lv - thr) * ex) >> 16);
            g1 = exp2_q14(gl);
        }
        m_g = g1;
        for (i = b; i < b + CTL && i < n; i++) {
            int32_t l = out[2u * i], r = out[2u * i + 1u], g = g0 + (((g1 - g0) * (int32_t)(i - b)) >> 4);
            uint32_t al, ar;
            if (mode && !(l | r) && (uint32_t)(m_fl.ic1 + 8) < 16u && (uint32_t)(m_fl.ic2 + 8) < 16u
                && (uint32_t)(m_fr.ic1 + 8) < 16u && (uint32_t)(m_fr.ic2 + 8) < 16u)
                m_fl.ic1 = m_fl.ic2 = m_fr.ic1 = m_fr.ic2 = 0;   /* (silence stays silence: no rounding hum of the states) */
            else if (mode) {                    /* (at half the level: the filter's band-pass state stays linear) */
                int32_t lp, bp, hp;
                svf_step(&m_c, &m_fl, l >> 1, &lp, &bp, &hp); l = (mode == 1 ? lp : hp) << 1;
                svf_step(&m_c, &m_fr, r >> 1, &lp, &bp, &hp); r = (mode == 1 ? lp : hp) << 1;
            }
            if (d) { l = (softclip((l * pre) >> 9) * post) >> 12; r = (softclip((r * pre) >> 9) * post) >> 12; }
            if (cm) {                           /* the level follower: 3 ms up, 150 ms down */
                int32_t p = (l < 0 ? -l : l) > (r < 0 ? -r : r) ? (l < 0 ? -l : l) : (r < 0 ? -r : r);
                int32_t e = clampi(p, 0, 65535) << 8;
                m_env += e > m_env ? (int32_t)(((int64_t)(e - m_env) * 495) >> 16) : (int32_t)(((int64_t)(e - m_env) * 10) >> 16);
                l = (int32_t)(((int64_t)l * g) >> 14); r = (int32_t)(((int64_t)r * g) >> 14);
            }
            l = (l * vol) >> 12; r = (r * vol) >> 12;
            al = (uint32_t)(l < 0 ? -l : l); ar = (uint32_t)(r < 0 ? -r : r);
            if (al > 26214u) { int32_t y = 26214 + softclip(((int32_t)al - 26214) * 5 / 2) / 5; l = l < 0 ? -y : y; }
            if (ar > 26214u) { int32_t y = 26214 + softclip(((int32_t)ar - 26214) * 5 / 2) / 5; r = r < 0 ? -y : y; }
            if (al > pk) pk = al;
            if (ar > pk) pk = ar;
            out[2u * i] = l; out[2u * i + 1u] = r;
        }
    }
    if (pk > ui_out_peak) ui_out_peak = pk;
}

OBV_FX_LINKAGE void fx_process(int32_t *out, const int32_t *cs, const int32_t *ds, const int32_t *rs, uint32_t n)
{
    uint32_t i, do_dly = dly_active != 0, do_rev = rev_active != 0;
    int32_t cm = cho_mix, dm = dly_mix, rm = rev_mix;
    for (i = 0; i < n; i++) {
        int32_t l = out[2u * i], r = out[2u * i + 1u];
        if (cm) {                               /* two taps a quarter turn apart on one line, around 12 ms */
            int32_t bl = (529 << 16) + tri_q(cho_ph) * cho_depth_k, br = (529 << 16) + tri_q(cho_ph + 0x40000000u) * cho_depth_k;
            cho_ph += cho_inc;
            line_write(&cho, cs[i]);
            l += (cho_read_q16_h(&cho, bl) * cm) >> 8;
            r += (cho_read_q16_h(&cho, br) * cm) >> 8;
        }
        if (do_dly) {                           /* ping-pong: the input enters left, the feedback crosses */
            int32_t wl, wr;
            if (dly_cur < dly_time) { dly_cur += 1 << 15; if (dly_cur > dly_time) dly_cur = dly_time; }
            else if (dly_cur > dly_time) { dly_cur -= 1 << 15; if (dly_cur < dly_time) dly_cur = dly_time; }
            wl = dly_read_q16(&dl, dly_cur);
            wr = dly_read_q16(&dr, dly_cur);
            dly_lpl += ((wr - dly_lpl) * dly_tone_k) >> 8;
            dly_lpr += ((wl - dly_lpr) * dly_tone_k) >> 8;
            line_write(&dl, ds[i] + ((dly_lpl * dly_fb) >> 15));
            line_write(&dr, (dly_lpr * dly_fb) >> 15);
            l += (wl * dm) >> 8;
            r += (wr * dm) >> 8;
        }
        if (do_rev) {
#if REV_DIV == 2u                               /* half rate: a pair in, the output interpolated between ticks */
            int32_t pl = rv_yl, pr = rv_yr, din = rs[i] * 2;
            if (!(fx_odd ^= 1u)) {
                rev_tick((rv_hold + din) >> 2);
                l += (((pl + rv_yl) >> 1) * rm) >> 8;
                r += (((pr + rv_yr) >> 1) * rm) >> 8;
            } else {
                l += (rv_yl * rm) >> 8;
                r += (rv_yr * rm) >> 8;
            }
            rv_hold = din;
#else
            rev_tick(rs[i]);
            l += (rv_yl * rm) >> 8;
            r += (rv_yr * rm) >> 8;
#endif
        }
        out[2u * i] = sat16(l);
        out[2u * i + 1u] = sat16(r);
    }
}
