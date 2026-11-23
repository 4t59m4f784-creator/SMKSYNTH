/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
static uint32_t test_clock;
#define DP_CLOCK() test_clock
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static uint32_t dc_irq_peak_ticks,dc_irq_overruns,dc_clips;
static uint8_t fm1_mc_on=1;
static uint32_t ui_render_peak_us,ui_render_overruns;
#define FM1_TICKS_PER_US 24u
#include "../firmware/src/smk_dual_core_ui.h"
int main(void){
 engine_init();pat_init(&pat);
 fm1_in.buttons=(1u<<BTN_GLO)|(1u<<BTN_SEL);
 assert(dc_diag_poll(fm1_in.buttons,0));fm1_in.buttons=0;assert(dc_diag_poll(0,0));
 host_enc[0]=-1;dc_diag_poll(0,0);assert(!dc_enabled);
 host_enc[1]=1;dc_diag_poll(0,0);assert(dp_state==DP_ARMED&&!dp_capture);
 dc_diag_poll(1u<<BTN_HOME,0);
 int32_t out[2u*HALF_FRAMES]={0};test_clock=DP_HZ*5u;app_audio(out,HALF_FRAMES);
 assert(dp_state==DP_ARMED&&!dp_halves);
 ctl_seq_run=1;app_audio(out,HALF_FRAMES);assert(dp_state==DP_RUNNING&&dp_capture);
 test_clock+=DP_WINDOW;app_audio(out,HALF_FRAMES);assert(dp_state==DP_DONE&&!dp_capture);
 fm1_in.buttons=(1u<<BTN_GLO)|(1u<<BTN_SEL);dc_diag_poll(fm1_in.buttons,0);fm1_in.buttons=0;dc_diag_poll(0,0);
 uint32_t elapsed=dp_elapsed;host_enc[2]=1;dc_diag_poll(0,0);test_clock+=DP_HZ;app_audio(out,HALF_FRAMES);
 assert(dp_elapsed==elapsed&&dp_state==DP_DONE&&!dp_capture);
 /* K2 cannot arm while the sequence is playing. */
 host_enc[1]=1;dc_diag_poll(0,0);assert(dp_state==DP_DONE);
 ctl_seq_run=0;host_enc[0]=1;dc_diag_poll(0,0);assert(dc_enabled);host_enc[1]=1;dc_diag_poll(0,0);assert(dp_state==DP_ARMED);
 puts("PASS: GLOBE+SCALE, K1, K2 arm, PATCH exit, PLAY start, automatic freeze, page browse, stopped-only rearm; 1C then 2C.");
}
