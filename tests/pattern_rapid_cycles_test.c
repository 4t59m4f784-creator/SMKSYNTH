/* SPDX-License-Identifier: GPL-3.0-only */
/* Local reproduction of fastest committed cycles and 6–90ms pad bursts. No user data included. */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);fm1_ms+=HALF_FRAMES*1000u/FS;}
static void meta(unsigned n){set.pat=n;set.tempo=ctl_tempo;snd_meta(n);pat_queue=0;ui_snd_swapped=0;}
int main(int argc,char **argv){
 if(argc!=2){fprintf(stderr,"Usage: rapid_cycles DECODED_SMK16P1_BANK\n");return 2;}
 engine_init();pf_init();set.tempo=ctl_tempo=120;set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();
 FILE*f=fopen(argv[1],"rb");assert(f);
 for(unsigned i=0;i<NPAT;i++){uint32_t h[4];fseek(f,4096+i*16384,SEEK_SET);assert(fread(h,4,4,f)==4);host_slot[i][0]=1;memcpy(host_slot[i]+4,h+1,8);assert(fread(host_slot[i]+16,1,h[1]+h[2],f)==h[1]+h[2]);}fclose(f);slot_lens_read();
 for(unsigned i=0;i<16;i++){pattern_t p;psnd_t snd;assert(!pattern_slot_load(i,&p,&snd));pattern_tempo_set(&p,(unsigned[]){90,120,75,160,105,140,100,135,85,110,95,150,80,125,115,170}[i]);assert(!pattern_slot_save(i,&p,&snd));}ctl_pattern_tempo=1;
 load_pattern(1);ctl_panic=0;endless_poll();ctl_seq_run=ctl_seq_restart=1;
 unsigned seen=ui_pat_swapped,requests=0,commits=0,selection=0,peak=0;
 /* First: fastest committed cycle, request the next pattern immediately
  * after acknowledgment. Commits still respect the downbeat. */
 for(unsigned cycle=0;cycle<128;cycle++){
  unsigned dest=cycle%16;pat_go(dest);requests++;unsigned old=ui_pat_swapped,loops=0;
  while(ui_pat_swapped==old&&loops++<3000){render();mout_poll();endless_poll();fx_sync();}
  assert(loops<3000);meta(dest);endless_poll();commits++;seen=ui_pat_swapped;
  assert(ctl_seq_run&&ctl_trk_on==15&&ctl_mix_off==0);
 }
 /* Then: 6–90ms pad bursts, replacement/cancellation requests, and a delayed
  * main-loop acknowledgment on both sides of the audio swap boundary. */
 unsigned intervals[]={1,2,3,7,15,1,1,2},due=0;
 for(unsigned n=0;n<40000;n++){
  render();mout_poll();endless_poll();fx_sync();
  if(n>=due){pat_go(selection++%16);requests++;due=n+intervals[requests%8];}
  if(n%3==0&&ui_pat_swapped!=seen){seen=ui_pat_swapped;assert(pat_queue);meta(pat_queue-1);commits++;}
  assert(ctl_seq_run&&ctl_trk_on==15&&ctl_mix_off==0);
  for(unsigned p=0;p<4;p++){assert(obv_engine_track[p].phase<=OBV_ENGINE_IN);assert(obv_pending_n[p]<=NHELD);}
  for(unsigned i=0;i<HALF_FRAMES*2;i++){unsigned v=audio[i]<0?(unsigned)-audio[i]:(unsigned)audio[i];if(v>peak)peak=v;}
 }
 printf("PASS rapid real-bank transitions: %u requests, %u downbeat commits, peak %u; transport/tracks/mix/engine state remained valid\n",requests,commits,peak);
}
