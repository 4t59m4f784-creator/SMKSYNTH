/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
/* Run with a decoded SMK16P1 bank to exercise private user data locally.
 * The regression fixtures below do not embed any user's bank. */
static void fixture(unsigned n,unsigned endless,unsigned bpm){
 pattern_t p;psnd_t snd;pat_init(&p);memset(&snd,0,sizeof snd);snd.magic=PSN2_MAGIC;
 for(unsigned t=0;t<NTRK;t++){
  patch_from_factory(&snd.p[t],&FACT_INIT);snd.p[t].v[P_ARP_MODE]=0;
  p.trk[t].len=t==0?23:t==1?15:16;p.trk[t].div=t==0?1:3;
  if(endless){p.trk[t].pad[0]=t==3?0xC1:0xC9;p.trk[t].pad[1]=96;}
  for(unsigned i=0;i<p.trk[t].len;i++){step_clear(&p.st[t][i]);p.st[t][i].note[0]=(uint8_t)(48+t*5+i%5);}
 }
 obv_drums_preset(&snd.p[3],0);mx_from_set(&snd.mx);pattern_tempo_set(&p,bpm);
 assert(!pattern_slot_save(n,&p,&snd));
}
static void transition(unsigned from,unsigned to,unsigned delay,unsigned tempo_source){
 endless_stop();seq_tick();pat_queue=ctl_pat_swap=ctl_snd_swap=ctl_mx_swap=0;
 set.tempo=ctl_tempo=100;set.pat=from;ctl_pattern_tempo=tempo_source;load_pattern(from);ctl_panic=0;endless_poll();
 ctl_seq_run=ctl_seq_restart=1;seq_tick();pat_go(to);
 unsigned ticks=0;while(ctl_pat_swap&&ticks++<100000)seq_tick();assert(!ctl_pat_swap);
 assert(ticks<100000&&ctl_seq_run);
 for(unsigned t=0;t<NTRK;t++)assert(sr[t].step==0&&sr[t].cnt==CTL);
 unsigned expected=tempo_source&&pattern_tempo(&pat)?pattern_tempo(&pat):100;
 assert(ctl_tempo==expected);
 if(end_mode)assert(end_clock_valid);
 for(unsigned n=0;n<delay;n++)seq_tick();
 seqrun_t before[NTRK];memcpy(before,sr,sizeof before);
 uint32_t restarts[NTRK];for(unsigned t=0;t<NTRK;t++)restarts[t]=end_restart[t];
 unsigned phase=end_clock_phase,beat=seq_beat_cnt,mode=end_mode;
 endless_poll();
 assert(!memcmp(before,sr,sizeof before));
 for(unsigned t=0;t<NTRK;t++)assert(restarts[t]==end_restart[t]);
 assert(end_clock_phase==phase&&seq_beat_cnt==beat&&end_mode==mode);
 /* No delayed restart on the following audio tick, even if a step wraps. */
 seq_tick();for(unsigned t=0;t<NTRK;t++)assert(sr[t].cnt!=CTL||before[t].cnt+CTL>=before[t].dur);
 if(end_mode){
  assert(end_clock_valid);
  /* A subsequent keyboard launch waits for the shared grid, not a free restart. */
  while(end_clock_phase% (quarter_len()/4u)<quarter_len()/8u)seq_tick();
  end_target_transpose=2;end_pending=1;unsigned prior=seq_beat_cnt;seq_tick();
  assert(end_pending&&seq_beat_cnt==prior+CTL);
  unsigned wait=0;while(end_pending&&wait++<quarter_len()/CTL)seq_tick();
  assert(!end_pending&&end_global_transpose==2);
 }
 endless_stop();seq_tick();ctl_seq_run=ctl_seq_restart=1;seq_tick();
 for(unsigned t=0;t<NTRK;t++)assert(sr[t].step==0&&sr[t].cnt==CTL);
}
int main(int argc,char**argv){
 engine_init();pf_init();set.tempo=ctl_tempo=120;set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();
 if(argc==2){
  FILE*f=fopen(argv[1],"rb");assert(f);
  for(unsigned i=0;i<NPAT;i++){uint32_t h[4];assert(!fseek(f,4096+i*16384,SEEK_SET));assert(fread(h,4,4,f)==4);assert(h[0]==0x544f4c53&&h[1]+h[2]+16<=16384);host_slot[i][0]=1;memcpy(host_slot[i]+4,h+1,8);assert(fread(host_slot[i]+16,1,h[1]+h[2],f)==h[1]+h[2]);}fclose(f);slot_lens_read();
  for(unsigned i=0;i<NPAT;i++)for(unsigned d=0;d<3;d++)for(unsigned m=0;m<2;m++)transition(i,6,d==0?0:d==1?55:900,m);
  puts("PASS: real bank, every source -> pattern 7, delayed main loop, both tempo sources");
 }else{
  for(unsigned a=0;a<2;a++)for(unsigned b=0;b<2;b++)for(unsigned m=0;m<2;m++)for(unsigned d=0;d<3;d++){
   fixture(0,a,100);fixture(1,b,173);slot_lens_read();transition(0,1,d==0?0:d==1?55:900,m);assert(ctl_tempo==(m?173:100));
  }
  puts("PASS: normal/Endless transition matrix, master/pattern BPM, delayed main loop");
 }
 return 0;
}
