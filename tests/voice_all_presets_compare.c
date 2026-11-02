/* SPDX-License-Identifier: GPL-3.0-only. Offline reference comparison; not a device timing test. */
#define main obv_unused_main
#ifndef OBV_TEST_SOURCE
#define OBV_TEST_SOURCE "../firmware/src/obervave.c"
#endif
#include OBV_TEST_SOURCE
#undef main
int main(void){int32_t out[HALF_FRAMES*2];uint64_t nonzero=0;engine_init();for(unsigned t=0;t<4;t++){trk_load(t,6);live_p[t].v[P_ARP_MODE]=A_OFF;set.mix_lvl[t]=t?0:100;set.mix_cho[t]=set.mix_dly[t]=set.mix_rev[t]=set.mix_drv[t]=0;}set.m_vol=100;set.m_dj=64;mix_apply();fx_sync();pat_init(&pat);ctl_vib=ctl_wheel_bend=ctl_bend=0;
 for(unsigned p=0;p<8;p++)for(unsigned notes=1;notes<=4;notes*=2){
  ctl_panic=1;memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);obv_voice_preset(&live_p[0],p);for(unsigned b=0;b<3;b++){memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);}
  for(unsigned b=0;b<240;b++){
   if(b==4||b==104)for(unsigned n=0;n<notes;n++)note_event_p(0,1,48+n*4,100);
   if(b==64||b==164)for(unsigned n=0;n<notes;n++)note_event_p(0,0,48+n*4,0);
   memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);for(unsigned i=0;i<HALF_FRAMES*2;i++)nonzero+=out[i]!=0;fwrite(out,1,sizeof out,stdout);
  }
 }
 fprintf(stderr,"8 presets x 1/2/4-note input, 1474560 frames, nonzero=%llu; mono presets retain native mono allocation. No physical CPU measured.\n",(unsigned long long)nonzero);return nonzero<100000;
}
