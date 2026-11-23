/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
#include <stdio.h>
int main(void){engine_init();pf_init();pat_init(&pat);set.tempo=90;set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();
 unsigned normal=0,super=0;int peak=0;
 for(unsigned mode=0;mode<4;mode++)for(unsigned wave=0;wave<2;wave++){
  ctl_panic=2;engine_events();
  for(unsigned p=0;p<NPART;p++){patch_from_factory(&live_p[p],&FACT_INIT);live_p[p].v[P_MODE]=mode;live_p[p].v[P_UNI_N]=8;live_p[p].v[P_SPREAD]=127;live_p[p].v[P_UNI_DET]=127;live_p[p].v[P_O1_LVL]=127;live_p[p].v[P_O2_LVL]=127;live_p[p].v[P_CUTOFF]=100;patch_sub_set(&live_p[p],wave,wave,100);}
  for(unsigned n=0;n<12;n++)note_event_p(0,1,40+n,127);
  for(unsigned b=0;b<128;b++){int32_t out[2*HALF_FRAMES];engine_audio(out,HALF_FRAMES);for(unsigned i=0;i<2*HALF_FRAMES;i++){assert(out[i]>=-32768&&out[i]<=32767);int a=out[i]<0?-out[i]:out[i];if(a>peak)peak=a;}}
  unsigned active=0;for(unsigned v=0;v<NV;v++)active+=vc[v].ae.stage!=E_IDLE;
  if(mode==M_POLY||mode==M_UNISON){assert(active==NV);normal=active;}
 }
 ctl_panic=2;engine_events();live_p[0].v[P_MODE]=M_POLY;live_p[0].v[P_O1_WAVE]=W_SUPER;
 for(unsigned n=0;n<12;n++)note_event_p(0,1,48+n,100);
 int32_t out[2*HALF_FRAMES];engine_audio(out,HALF_FRAMES);super=super_count();assert(super==SUPER_LIMIT);
 /* The actual split/layer router allocates ordinary voices with SUB state. */
 ctl_panic=2;engine_events();for(unsigned p=0;p<NPART;p++){patch_from_factory(&live_p[p],&FACT_INIT);patch_sub_set(&live_p[p],p&1,p>>1,75);}
 for(unsigned p=0;p<NPART;p++)note_event_p(p,1,48+p*7,100);
 engine_audio(out,HALF_FRAMES);for(unsigned p=0;p<NPART;p++){unsigned found=0;for(unsigned v=0;v<NV;v++)if(vc[v].ae.stage!=E_IDLE&&vc[v].part==p){assert(vc[v].sub_gain>0);found++;}assert(found);}
 printf("PASS: 8 normal/UNISON voices and existing %u SUPER ceiling; four-part SUB ownership; rapid mode/wave stress; bounded stereo peak %d.\n",super,peak);
}
