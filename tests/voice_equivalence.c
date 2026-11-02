/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#ifndef OBV_TEST_SOURCE
#define OBV_TEST_SOURCE "../firmware/src/obervave.c"
#endif
#include OBV_TEST_SOURCE
#undef main
int main(void){
 int32_t audio[HALF_FRAMES*2];uint64_t nonzero=0;
 engine_init();for(unsigned t=0;t<4;t++){trk_load(t,t*6);live_p[t].v[P_ARP_MODE]=A_OFF;set.mix_lvl[t]=100;set.mix_cho[t]=55;set.mix_dly[t]=40;set.mix_rev[t]=60;}
 set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();pat_init(&pat);
 obv_voice_preset(&live_p[1],0);obv_voice_preset(&live_p[2],2);
 for(unsigned b=0;b<1600;b++){
  unsigned t=(b/64)%4,n=48+(b/128)%20;
  if(b%64==8)note_event_p(t,1,n,105);
  if(b%64==52)note_event_p(t,0,n,0);
  if(b%150==0){ctl_wheel_part=t;ctl_vib=b%128;ctl_wheel_bend=(int)(b%8192)-4096;}
#ifdef OBV_VD1_WHEEL_EMULATION
  /* DSP-only comparison: emulate VD1 focus gating. Real controller retention
   * is tested independently in vd2_controller_audio_test.c. */
  for(unsigned p=0;p<NPART;p++)ctl_formant_wheel[p]=p==ctl_wheel_part?ctl_vib:0;
#endif
  memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);
  for(unsigned i=0;i<HALF_FRAMES*2;i++)if(audio[i])nonzero++;
  fwrite(audio,1,sizeof audio,stdout);
 }
 if(nonzero<100000){fprintf(stderr,"FAIL: silent VOICE regression\n");return 1;}
 fprintf(stderr,"AUDIBLE VOICE MIX: %llu nonzero samples\n",(unsigned long long)nonzero);
 return 0;
}
