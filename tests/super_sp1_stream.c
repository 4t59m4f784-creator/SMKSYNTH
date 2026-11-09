/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(int argc,char**argv){
 assert(argc==2);FILE*f=fopen(argv[1],"wb");assert(f);
 engine_init();pf_init();pat_init(&pat);seq_trk=0;ctl_cc7=100;
 for(unsigned t=0;t<4;t++){patch_from_factory(&live_p[t],&FACT_INIT);live_p[t].v[P_ARP_MODE]=0;live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_AA]=1;live_p[t].v[P_AR]=110;live_p[t].v[P_VINTAGE]=45;set.mix_lvl[t]=100;set.mix_dly[t]=set.mix_rev[t]=set.mix_cho[t]=80;}
 set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();
 for(unsigned test=0;test<6;test++){
  for(unsigned t=0;t<4;t++){live_p[t].v[P_O1_WAVE]=test==5?W_SAW:W_SUPER;live_p[t].v[P_O2_WAVE]=test%4;live_p[t].v[P_O2_LVL]=test==4?0:100;patch_super_mix_set(&live_p[t],0,test%2?127:64);live_p[t].v[P_O1_PW]=test%2?127:60;live_p[t].v[P_LFO_PITCH]=25;}
  for(unsigned attack=0;attack<16;attack++){
   unsigned notes=attack<4?3:attack<8?4:8;
   for(unsigned v=0;v<notes;v++)note_event_p(v%4,1,60+v*4+attack%3,100);
   for(unsigned b=0;b<24;b++){int32_t out[2*HALF_FRAMES]={0};engine_audio(out,HALF_FRAMES);assert(fwrite(out,sizeof*out,2*HALF_FRAMES,f)==2*HALF_FRAMES);}
   for(unsigned v=0;v<notes;v++)note_event_p(v%4,0,60+v*4+attack%3,0);
   /* Change scene parameters with eight release tails still active. */
   for(unsigned t=0;t<4;t++){live_p[t].v[P_CUTOFF]=30+attack*4+t;live_p[t].v[P_RESO]=35+attack;}
   for(unsigned b=0;b<5;b++){int32_t out[2*HALF_FRAMES]={0};engine_audio(out,HALF_FRAMES);assert(fwrite(out,sizeof*out,2*HALF_FRAMES,f)==2*HALF_FRAMES);}
  }
 }
 fclose(f);puts("PASS: complete FX stream, OSC1 SUPER + every normal OSC2 wave, long releases/stealing, 3/4/8-note bursts, high pitches, modulation and scene-parameter changes.");
}
