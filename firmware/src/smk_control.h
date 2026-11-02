/* SPDX-License-Identifier: GPL-3.0-only */
#pragma once
#include <stdint.h>
/* Fractional ADC accumulator: single-count movements are never discarded. */
static int32_t smk_wheel_filter(int32_t *q, int32_t raw) {
    *q += (raw * 256 - *q) / 4;
    return (*q + 128) / 256;
}
/* SMK only: ~5.8 ms time constant at CTL=16, FS=44100. Q8 semitone target. */
static int32_t smk_bend_slew(int32_t *q, int32_t target) {
    int32_t d = target * 256 - *q;
    if(d > -16 && d < 16)*q = target * 256;
    else *q += d / 16;
    return (*q + (*q < 0 ? -128 : 128)) / 256;
}
