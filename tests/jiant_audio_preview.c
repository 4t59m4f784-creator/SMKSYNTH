/* SPDX-License-Identifier: GPL-3.0-only. Offline dry audition, no hardware timing claim. */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
int main(void){engine_init();for(unsigned t=0;t<4;t++){trk_load(t,6);set.mix_lvl[t]=t?0:100;set.mix_cho[t]=set.mix_dly[t]=set.mix_rev[t]=set.mix_drv[t]=0;}set.m_vol=100;set.m_dj=64;set.m_drive=set.m_comp=0;fx_sync();mix_apply();pat_init(&pat);int32_t out[HALF_FRAMES*2];unsigned presets[]={0,2,3};
 for(unsigned p=0;p<3;p++){
  ctl_panic=1;for(unsigned b=0;b<2000;b++)engine_audio(out,HALF_FRAMES);jmx_reset();obv_voice_preset(&live_p[0],presets[p]);ctl_formant_wheel[0]=0;
  for(unsigned b=0;b<1050;b++){
   if(b==12){note_event_p(0,1,60,100);if(!p){note_event_p(0,1,64,100);note_event_p(0,1,67,100);}}
   if(b==250&&p)note_event_p(0,1,72,100);
   if(b==480){if(p)note_event_p(0,1,79,100);else ctl_formant_wheel[0]=64;}
   if(b==650&&!p)ctl_formant_wheel[0]=127;
   if(b==800){static const unsigned notes[]={60,64,67,72,79};for(unsigned n=0;n<5;n++)note_event_p(0,0,notes[n],0);}
   memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);fwrite(out,1,sizeof out,stdout);
  }
 }
 return 0;}
