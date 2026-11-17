/* SPDX-License-Identifier: GPL-3.0-only */
/* DSP-only -O2 object; never enable -O2 for USB/main. State remains CPU0 owned. */
#include <stdint.h>
#include "obv_dsp.h"
#define CTL 16u
#include "obv_fx_shared.h"
#define OBV_FX_LINKAGE
#include "obv_fx_process.h"
