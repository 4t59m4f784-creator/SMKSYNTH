/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
static uint32_t clock_ticks;
#define OBV_DUAL_CORE 1
#define DP_CLOCK() clock_ticks
#include "../firmware/src/smk_audio_profile.h"
int main(void){
 dp_arm();assert(dp_state==DP_ARMED&&!dp_capture);
 clock_ticks=123456;dp_window_tick(0);assert(dp_state==DP_ARMED&&!dp_capture);
 dp_window_tick(1);assert(dp_state==DP_RUNNING&&dp_capture&&dp_elapsed==0);
 dp_begin(DP_SUPER);clock_ticks+=400;dp_nested+=80;dp_end(DP_SUPER);
 assert(dp_cur[DP_SUPER]==320);dp_half(1000,80);
 assert(dp_halves==1&&dp_sum[DP_SUPER]==320&&dp_peak[DP_SUPER]==320&&dp_sum[DP_IO]==600&&dp_sum[DP_NEST]==80);
 clock_ticks=dp_start+DP_WINDOW;dp_window_tick(1);assert(dp_state==DP_DONE&&!dp_capture);
 uint32_t halves=dp_halves,total=dp_sum[DP_SUPER];dp_half(5000,0);dp_begin(DP_SUPER);clock_ticks+=100;dp_end(DP_SUPER);
 assert(dp_halves==halves&&dp_sum[DP_SUPER]==total);
 dp_arm();assert(!dp_halves&&!dp_sum[DP_SUPER]);clock_ticks=UINT32_MAX-20;dp_window_tick(0);assert(dp_state==DP_ARMED);
 dp_window_tick(1);clock_ticks=dp_start+DP_WINDOW-1;dp_window_tick(1);assert(dp_capture);
 clock_ticks++;dp_window_tick(1);assert(dp_state==DP_DONE&&!dp_capture);
 dp_arm();dp_window_tick(1);clock_ticks+=24000;dp_window_tick(0);assert(dp_state==DP_DONE&&dp_elapsed==24000);
 puts("PASS: arm waits for PLAY; 30-second wall-clock window freezes; nested IRQ subtraction; residual IO; reset; timer wrap; early transport stop.");
}
