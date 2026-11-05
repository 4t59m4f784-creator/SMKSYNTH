/* SPDX-License-Identifier: GPL-3.0-only */
/* Isolated -O2 DSP object. The USB/main firmware keeps its tested -Os flags. */
#include <stdint.h>
#include "obv_osc_type.h"
#define CTL 16u
#define OBV_SUPER_KERNEL 1
#include "obv_supersaw.h"
