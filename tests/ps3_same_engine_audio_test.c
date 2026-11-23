/* SPDX-License-Identifier: GPL-3.0-only */
/* PCM comparison against PS2 for recalls without an engine change. */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t pcm[HALF_FRAMES*2];
static void fixture(unsigned slot){
 pattern_t p;psnd_t snd;pat_init(&p);memset(&snd,0,sizeof snd);snd.magic=PSN2_MAGIC;mx_from_set(&snd.mx);
 for(unsigned t=0;t<4;t++){patch_from_factory(&snd.p[t],&FACT_INIT);snd.p[t].v[P_ARP_MODE]=A_OFF;snd.p[t].v[P_MODE]=t==0?M_MONO:M_POLY;snd.p[t].v[P_CUTOFF]=(uint8_t)(70+slot*15);p.trk[t].len=16;
 for(unsigned i=0;i<16;i++){step_clear(&p.st[t][i]);p.st[t][i].note[0]=(uint8_t)(48+t*5+i%4+slot*2);}}
 obv_drums_preset(&snd.p[3],0);for(unsigned i=0;i<16;i++)drum_pattern_set(&p,3,i,i%4==0?3:1);
 assert(!pattern_slot_save(slot,&p,&snd));
}
int main(void){
 engine_init();pf_init();set.tempo=ctl_tempo=120;set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();
 fixture(0);fixture(1);slot_lens_read();load_pattern(0);ctl_panic=0;endless_poll();
 for(unsigned i=0;i<8;i++){memset(pcm,0,sizeof pcm);engine_audio(pcm,HALF_FRAMES);}
 ctl_seq_run=ctl_seq_restart=1;
 for(unsigned i=0;i<1800;i++){
  if(i%350==100)pat_go((i/350+1)%2);
  memset(pcm,0,sizeof pcm);engine_audio(pcm,HALF_FRAMES);
  if(pat_queue&&!ctl_pat_swap){set.pat=pat_queue-1;pat_queue=0;snd_meta(set.pat);}
  endless_poll();mout_poll();assert(fwrite(pcm,sizeof pcm,1,stdout)==1);
 }
 return 0;
}
