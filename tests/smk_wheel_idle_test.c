/* SPDX-License-Identifier: GPL-3.0-only
 * Real app_idle, controlled 1ms host yields; NOT a measurement on the JieLi. */
#ifndef OBV_TEST_APP
#define OBV_TEST_APP "../firmware/src/obervave.c"
#endif
#define main obv_host_main
#include OBV_TEST_APP
#undef main
#include <assert.h>
int main(void){
 idle_ok=1;page=PG_PERFORM;fm1_in.frames=1000;fm1_ms=3000;
 fm1_in.wheel[0]=535;fm1_in.wheel[1]=620;smk_wheels();
 fm1_in.wheel[0]=635;smk_wheels();fm1_in.wheel[0]=435;smk_wheels();fm1_in.wheel[0]=535;smk_wheels();
 unsigned updates=0,max_gap=0,last_ms=fm1_ms;int last=ctl_wheel_bend;
 for(unsigned i=0;i<50;i++){
  fm1_ms++;fm1_in.wheel[0]=560+i;app_idle();
  if(ctl_wheel_bend!=last){unsigned gap=fm1_ms-last_ms;if(gap>max_gap)max_gap=gap;last_ms=fm1_ms;last=ctl_wheel_bend;updates++;}
 }
 if(fm1_ms-last_ms>max_gap)max_gap=fm1_ms-last_ms;
#ifdef EXPECT_IDLE_WHEELS
 assert(updates==50&&max_gap==1);
 fm1_in.wheel[0]=535;app_idle();assert(ctl_wheel_bend==0);
 fm1_in.wheel[0]=470;app_idle();assert(ctl_wheel_bend<0);
 fm1_in.wheel[0]=535;app_idle();assert(ctl_wheel_bend==0);
 fm1_in.wheel[1]=500;app_idle();assert(ctl_vib>0);
 fm1_in.wheel[1]=620;app_idle();assert(ctl_vib==0);
 idle_ok=0;fm1_in.wheel[0]=600;app_idle();assert(ctl_wheel_bend==0);
 idle_ok=1;app_idle();assert(ctl_wheel_bend>0);
 for(int i=0;i<4;i++)assert(host_write_count[i]==0);
#else
 assert(updates==0&&max_gap==50);smk_wheels();assert(ctl_wheel_bend>0);
#endif
 printf("WHEEL IDLE %s: %u updates / 50ms synthetic LCD yield; max gap %ums. Physical timing remains unmeasured.\n",updates?"PF9":"PF8 baseline",updates,max_gap);
 return 0;}
