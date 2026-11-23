/* SPDX-License-Identifier: GPL-3.0-only */
/* Local reproduction of 10 ordered cycles with varying BPM. No user data included. */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);fm1_ms+=HALF_FRAMES*1000u/FS;}
static void meta(unsigned n){set.pat=n;set.tempo=ctl_tempo;snd_meta(n);pat_queue=0;ui_snd_swapped=0;}
int main(int argc,char **argv){
 if(argc!=2){fprintf(stderr,"Usage: bank_cycles DECODED_SMK16P1_BANK\n");return 2;}
 engine_init();pf_init();set.tempo=ctl_tempo=120;set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();
 FILE*f=fopen(argv[1],"rb");assert(f);
 for(unsigned i=0;i<NPAT;i++){uint32_t h[4];fseek(f,4096+i*16384,SEEK_SET);assert(fread(h,4,4,f)==4);host_slot[i][0]=1;memcpy(host_slot[i]+4,h+1,8);assert(fread(host_slot[i]+16,1,h[1]+h[2],f)==h[1]+h[2]);}fclose(f);slot_lens_read();
 for(unsigned i=0;i<16;i++){
  pattern_t p;psnd_t s;assert(!pattern_slot_load(i,&p,&s));printf("P%u",i+1);
  for(unsigned t=0;t<4;t++){const step_t*k=&p.st[t][0];printf(" T%u eng%u mode%u arp%u note%u gate%u chance%u nudge%u vol%u",t,obv_patch_engine(&s.p[t]),s.p[t].v[P_MODE],s.p[t].v[P_ARP_MODE],k->note[0],k->gate,k->chance,k->nudge,s.p[t].v[P_VOLUME]);}puts("");
 }
 for(unsigned i=0;i<16;i++){pattern_t p;psnd_t snd;assert(!pattern_slot_load(i,&p,&snd));pattern_tempo_set(&p,(unsigned[]){90,120,75,160,105,140,100,135,85,110,95,150,80,125,115,170}[i]);assert(!pattern_slot_save(i,&p,&snd));}ctl_pattern_tempo=1;
 load_pattern(1);ctl_panic=0;endless_poll();ctl_seq_run=ctl_seq_restart=1;
 for(unsigned cycle=0;cycle<160;cycle++){
  unsigned dest=cycle%16;pat_go(dest);unsigned swapped=ui_pat_swapped,loops=0;
  while(ui_pat_swapped==swapped&&loops++<3000){render();mout_poll();endless_poll();fx_sync();}
  assert(loops<3000);meta(dest);endless_poll();
  unsigned gates=0;for(unsigned v=0;v<NV;v++)gates+=vc[v].gate;printf("SWAP%u ->%u frames%u trkon%x mute%x gates%u sr0=%u/%u nsnd%u phase%u engines%u/%u\n",cycle,dest+1,loops*HALF_FRAMES,ctl_trk_on,ctl_mix_off,gates,sr[0].step,sr[0].cnt,sr[0].nsnd,obv_engine_track[0].phase,obv_engine_track[0].active,obv_engine_track[0].target);
  for(unsigned n=0;n<20;n++){render();mout_poll();endless_poll();fx_sync();}
 }
}
