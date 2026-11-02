/* SPDX-License-Identifier: GPL-3.0-only. Host cost comparison, never a device CPU measurement. */
#define OBV_FORMANT_PROFILE 1
#define main obv_host_main
#ifndef OBV_TEST_SOURCE
#define OBV_TEST_SOURCE "../firmware/src/obervave.c"
#endif
#include OBV_TEST_SOURCE
#undef main
#include <time.h>
int main(void){int32_t out[HALF_FRAMES*2];engine_init();for(unsigned t=0;t<4;t++){trk_load(t,6);set.mix_lvl[t]=100;}set.m_vol=100;set.m_dj=64;mix_apply();fx_sync();pat_init(&pat);obv_voice_preset(&live_p[0],0);
 for(unsigned i=0;i<3;i++)engine_audio(out,HALF_FRAMES);
 for(unsigned v=0;v<8;v++)note_event_p(0,1,48+v*3,100);
 clock_t a=clock();uint64_t energy=0;for(unsigned b=0;b<4000;b++){memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);energy+=(uint64_t)((int64_t)out[0]*out[0]);}
 printf("host_seconds=%.6f eight_voice_blocks=4000 energy=%llu\n",(double)(clock()-a)/CLOCKS_PER_SEC,(unsigned long long)energy);
#ifdef VD2_BENCH
 printf("fres_coef_updates=%u cached_state_bytes=%zu\n",fmt_coef_updates,sizeof fmt_state);
#endif
 return 0;}
