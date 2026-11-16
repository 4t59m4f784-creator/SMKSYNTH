/* SPDX-License-Identifier: GPL-3.0-only */
/* DC2: one 30-second wall-clock window, armed while stopped and started by
 * transport. CPU0 stage times exclude nested TIMER5 work. CPU1 is separate. */
#pragma once
#ifndef DP_CLOCK
#ifdef OBV_HOST
#define DP_CLOCK() 0u
#else
#define DP_CLOCK() fm1_ticks()
#endif
#endif
#define DP_HZ 24000000u
#define DP_WINDOW (30u*DP_HZ)
enum { DP_CTRL,DP_ANALOG,DP_SUPER,DP_DRUM,DP_COPY,DP_WAIT,DP_FX,DP_MIX,DP_IO,DP_NEST,DP_N };
enum { DP_IDLE,DP_ARMED,DP_RUNNING,DP_DONE };
#if OBV_DUAL_CORE
static volatile uint8_t dp_state,dp_capture;
static volatile uint32_t dp_nested,dp_start,dp_elapsed,dp_halves;
static uint32_t dp_cur[DP_N],dp_sum[DP_N],dp_peak[DP_N],dp_t[DP_N],dp_nt[DP_N];
static void dp_arm(void)
{
    dp_capture=0;dp_state=DP_IDLE;
    for(unsigned i=0;i<DP_N;i++)dp_cur[i]=dp_sum[i]=dp_peak[i]=0;
    dp_halves=dp_elapsed=0;dp_state=DP_ARMED;
}
static void dp_window_tick(unsigned run)
{
    uint32_t now=DP_CLOCK();
    if(dp_state==DP_ARMED&&run){dp_start=now;dp_state=DP_RUNNING;}
    if(dp_state==DP_RUNNING){
        dp_elapsed=now-dp_start;
        if(!run||dp_elapsed>=DP_WINDOW)dp_state=DP_DONE;
    }
    dp_capture=dp_state==DP_RUNNING;
    for(unsigned i=0;i<DP_N;i++)dp_cur[i]=0;
}
static inline void dp_begin(unsigned s)
{if(dp_capture){dp_t[s]=DP_CLOCK();dp_nt[s]=dp_nested;}}
static inline void dp_end(unsigned s)
{if(dp_capture)dp_cur[s]+=DP_CLOCK()-dp_t[s]-(dp_nested-dp_nt[s]);}
static void dp_half(uint32_t elapsed,uint32_t nested)
{
    if(!dp_capture)return;
    uint32_t accounted=0;
    dp_cur[DP_NEST]=nested;
    for(unsigned i=0;i<DP_IO;i++)accounted+=dp_cur[i];
    dp_cur[DP_IO]=elapsed>accounted+nested?elapsed-accounted-nested:0;
    for(unsigned i=0;i<DP_N;i++){
        dp_sum[i]+=dp_cur[i];if(dp_cur[i]>dp_peak[i])dp_peak[i]=dp_cur[i];
    }
    dp_halves++;
}
#define DP_RECORD (dp_capture)
#else
#define dp_begin(s) ((void)0)
#define dp_end(s) ((void)0)
#define dp_window_tick(run) ((void)0)
#define DP_RECORD 0
#endif
