/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}
static uint64_t energy(void){uint64_t e=0;for(unsigned i=0;i<HALF_FRAMES*2;i++)e+=(int64_t)audio[i]*audio[i];return e;}
static unsigned cards(void){unsigned n=0;for(unsigned i=0;i<NV;i++)n+=!!vc[i].gate;return n;}
int main(int argc,char **argv){
 engine_init();for(unsigned t=0;t<4;t++){trk_load(t,0);set.mix_lvl[t]=100;}set.m_vol=100;set.m_dj=64;set.tempo=120;
 fx_sync();mix_apply();pat_init(&pat);pf_init();
 for(unsigned k=0;k<NDRUMKIT;k++){
  obv_drums_preset(&live_p[0],k);render();render();assert(obv_engine_track[0].active==OBV_DRUMS);
  for(unsigned p=0;p<NPARAM;p++)assert(live_p[0].v[p]<=PDEF[p].max);
  for(unsigned l=0;l<8;l++){
   drums_reset(0);note_event_p(0,1,DRUM_NOTE[l],100);uint64_t e=0;
   for(unsigned b=0;b<80;b++){render();e+=energy();}
   assert(e>1000000);assert(!cards());
  }
 }
 obv_drums_preset(&live_p[0],0);drums_reset(0);
 note_event_p(0,1,46,127);render();assert(drum_hit[0][DV_HATO].core.active);
 note_event_p(0,1,42,127);render();assert(drum_hit[0][DV_HATO].core.choke);
 for(unsigned b=0;b<10;b++)render();assert(!drum_hit[0][DV_HATO].core.active);
 drums_reset(0);note_event_p(0,1,46,127);note_event_p(0,1,42,127);render();
 assert(!drum_hit[0][DV_HATO].core.active&&!drum_hit[0][DV_HATO].core.trig);
 /* A very short MIDI note during engine transition is still a one-shot hit. */
 obv_patch_set_engine(&live_p[0],OBV_ANALOG);render();render();obv_drums_preset(&live_p[0],0);
 note_event_p(0,1,36,100);note_event_p(0,0,36,0);render();render();assert(drums_busy(0));
 assert(!engine_quiet());ctl_panic=1;render();assert(!drums_busy(0));
 /* Velocity, tail ownership and level-zero behaviour. */
 uint64_t e[2];for(unsigned j=0;j<2;j++){drums_reset(0);note_event_p(0,1,36,j?127:45);e[j]=0;for(unsigned b=0;b<60;b++){render();e[j]+=energy();}}
 assert(e[1]>e[0]*3u);note_event_p(0,0,36,0);render();assert(drums_busy(0));
 drums_reset(0);live_p[0].v[DP_LEVEL[0]]=0;note_event_p(0,1,36,127);render();assert(!drums_busy(0));live_p[0].v[DP_LEVEL[0]]=100;
 /* ANALOG and DRUM-X coexist. The drum part allocates zero melodic cards. */
 patch_from_factory(&live_p[1],&FACTORY[6]);live_p[1].v[P_ARP_MODE]=A_OFF;
 patch_from_factory(&live_p[2],&FACTORY[0]);live_p[2].v[P_MODE]=M_POLY;live_p[2].v[P_ARP_MODE]=A_OFF;obv_drums_preset(&live_p[3],1);render();render();
 note_event_p(1,1,48,100);note_event_p(2,1,60,100);note_event_p(2,1,64,100);render();
 unsigned before=cards();assert(before>=3);
 for(unsigned t=0;t<4;t+=3)for(unsigned l=0;l<8;l++)note_event_p(t,1,DRUM_NOTE[l],100);
 render();assert(cards()==before&&drums_busy(0)&&drums_busy(3));
 ctl_mix_cho[0]=ctl_mix_dly[0]=ctl_mix_rev[0]=ctl_mix_drv[0]=127;render();
 uint64_t sends[4]={0};for(unsigned i=0;i<HALF_FRAMES;i++){
  sends[0]+=snd_c[i]<0?-snd_c[i]:snd_c[i];sends[1]+=snd_d[i]<0?-snd_d[i]:snd_d[i];
  sends[2]+=snd_r[i]<0?-snd_r[i]:snd_r[i];sends[3]+=snd_v[i]<0?-snd_v[i]:snd_v[i];}
 for(unsigned i=0;i<4;i++)assert(sends[i]>0);
 for(unsigned i=0;i<4;i++)assert(host_write_count[i]==0);
 printf("PASS: five kits / eight instruments emit nonzero audio; no melodic cards; velocity, one-shot note-off, same-block hat choke, panic/quiet guard, all sends and two-engine coexistence. hit pool=%zu bytes.\n",sizeof drum_hit);
 if(argc>1){
  FILE *f=fopen(argv[1],"wb");assert(f);unsigned kit=argc>2?(unsigned)atoi(argv[2]):0;
  ctl_panic=1;for(unsigned b=0;b<2500;b++)render();
  for(unsigned t=0;t<4;t++){ctl_mix_lvl[t]=t?0:100;ctl_mix_cho[t]=ctl_mix_dly[t]=ctl_mix_rev[t]=ctl_mix_drv[t]=0;}
  obv_drums_preset(&live_p[0],kit);render();render();
  for(unsigned b=0;b<1800;b++){
   if(b<8*140 && b%140==0)note_event_p(0,1,DRUM_NOTE[b/140],110);
   render();fwrite(audio,1,sizeof audio,f);
  }fclose(f);
 }
 return 0;
}
