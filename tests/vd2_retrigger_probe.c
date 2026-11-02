/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#ifndef OBV_TEST_SOURCE
#define OBV_TEST_SOURCE "../firmware/src/obervave.c"
#endif
#include OBV_TEST_SOURCE
#undef main
static int peak(int32_t*out){int p=0;for(unsigned i=0;i<CTL*2;i++){int a=out[i]<0?-out[i]:out[i];if(a>p)p=a;}return p;}
int main(void){int32_t out[CTL*2];engine_init();
 for(unsigned kit=0;kit<5;kit++)for(unsigned lane=0;lane<8;lane++){
  obv_drums_preset(&live_p[0],kit);drums_reset(0);
  drums_trigger(0,DRUM_NOTE[lane],127);for(unsigned b=0;b<25000;b++){memset(out,0,sizeof out);drums_render(0,out);if(!drums_busy(0))break;}
  drums_trigger(0,DRUM_NOTE[lane],8);memset(out,0,sizeof out);drums_render(0,out);int old=peak(out);
  drums_reset(0);drums_trigger(0,DRUM_NOTE[lane],8);memset(out,0,sizeof out);drums_render(0,out);int cold=peak(out);
  printf("kit=%u lane=%u soft_after_loud_peak=%d cold_soft_peak=%d\n",kit,lane,old,cold);
 }
 return 0;}
