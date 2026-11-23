/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t samples[HALF_FRAMES*2];
static void render(unsigned n){memset(samples,0,sizeof samples);engine_audio(samples,n);}
static void fixture(unsigned slot,unsigned engine){
 pattern_t p;psnd_t snd;pat_init(&p);memset(&snd,0,sizeof snd);snd.magic=PSN2_MAGIC;mx_from_set(&snd.mx);
 for(unsigned t=0;t<4;t++){patch_from_factory(&snd.p[t],&FACT_INIT);snd.p[t].v[P_ARP_MODE]=A_OFF;snd.p[t].v[P_MODE]=M_POLY;p.trk[t].len=16;p.trk[t].div=3;
 for(unsigned i=0;i<16;i++){step_clear(&p.st[t][i]);p.st[t][i].note[0]=(uint8_t)(t==3?43:48+t*5);}}
 if(engine==OBV_DRUMS){obv_drums_preset(&snd.p[3],0);for(unsigned i=0;i<16;i++)drum_pattern_set(&p,3,i,1);}
 assert(!pattern_slot_save(slot,&p,&snd));
}
static void begin(unsigned slot){
 endless_stop();render(HALF_FRAMES);pat_queue=ctl_pat_swap=ctl_snd_swap=ctl_mx_swap=0;
 set.pat=slot;load_pattern(slot);ctl_panic=0;endless_poll();
 for(unsigned i=0;i<4;i++)render(HALF_FRAMES);
 ctl_seq_run=ctl_seq_restart=1;render(HALF_FRAMES);
}
static void transition(unsigned source,unsigned dest,unsigned engine){
 begin(source);pat_go(dest);unsigned n=0;
 while(ctl_pat_swap&&n++<100000){
  unsigned old=ui_pat_swapped;render(CTL);
  if(ui_pat_swapped!=old){
   assert(obv_engine_track[3].active==engine);
   assert(obv_engine_track[3].phase==OBV_ENGINE_READY&&obv_engine_track[3].gain==32768);
   assert(!obv_pending_n[3]);
   if(engine==OBV_ANALOG){unsigned gates=0;for(unsigned v=0;v<NV;v++)if(vc[v].part==3&&vc[v].gate&&vc[v].seq_owned){gates++;assert(vc[v].ae.level>0);}assert(gates);}
   else assert(drum_stamp[3][0]);
   unsigned other=0;for(unsigned v=0;v<NV;v++)other+=vc[v].gate&&vc[v].part!=3;assert(other);
   assert(sr[3].step==0&&sr[3].cnt==CTL);
  }
 }
 assert(n<100000&&!ctl_pat_swap);
 for(unsigned i=0;i<128;i++)render(CTL);
 assert(!obv_pending_n[3]&&obv_engine_track[3].phase==OBV_ENGINE_READY);
}
int main(int argc,char**argv){
 engine_init();pf_init();set.tempo=ctl_tempo=120;set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();
 if(argc==2){
  FILE*f=fopen(argv[1],"rb");assert(f);
  for(unsigned i=0;i<NPAT;i++){uint32_t h[4];fseek(f,4096+i*16384,SEEK_SET);assert(fread(h,4,4,f)==4);assert(h[1]+h[2]+16<=16384);host_slot[i][0]=1;memcpy(host_slot[i]+4,h+1,8);assert(fread(host_slot[i]+16,1,h[1]+h[2],f)==h[1]+h[2]);}fclose(f);slot_lens_read();
  for(unsigned i=0;i<NPAT;i++)transition(i,0,OBV_ANALOG);
  puts("PASS PS3: every real-bank source -> pattern 1, first kick envelope active in swap CTL, no pending/fade delay");
 }else{
  fixture(0,OBV_ANALOG);fixture(1,OBV_DRUMS);slot_lens_read();
  transition(0,1,OBV_DRUMS);transition(1,0,OBV_ANALOG);transition(0,0,OBV_ANALOG);transition(1,1,OBV_DRUMS);
  /* Same-engine recall is a strict no-op for voice/render/guard state. */
  for(unsigned v=0;v<NV;v++)samples[v]=vc[v].gate;
  obv_engine_track_t before=obv_engine_track[3];unsigned g=guard_left[3];
  seq_pattern_engine_commit(3,OBV_DRUMS);assert(!memcmp(&before,&obv_engine_track[3],sizeof before)&&g==guard_left[3]);
  for(unsigned v=0;v<NV;v++)assert((unsigned)samples[v]==vc[v].gate);
  /* No clearing of the shared FX tank; cross-engine commit only touches its part. */
  fx_mem[100]=1234;pattern_tail[3][0]=3200;pattern_tail[3][1]=-1600;
  seq_pattern_engine_commit(3,OBV_ANALOG);assert(fx_mem[100]==1234&&guard_left[3]==STEAL_FADE&&guard_tail[3][0]==3200&&guard_tail[3][1]==-1600);
  puts("PASS PS3: analog/drums/same-engine transitions; onset within existing CTL; short outgoing guard; FX/other parts preserved");
 }
 return 0;
}
