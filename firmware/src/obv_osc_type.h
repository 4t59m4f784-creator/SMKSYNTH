/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef OBV_OSC_TYPE_H
#define OBV_OSC_TYPE_H
typedef struct {
    uint32_t ph;     /* wrapping phase */
    int32_t prev;    /* delayed sample, still open to the wrap correction */
} osc_t;
#endif
