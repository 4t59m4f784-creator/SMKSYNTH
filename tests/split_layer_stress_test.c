/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(unsigned n){while(n--){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}}
static unsigned gates(void){unsigned n=0;for(unsigned i=0;i<NV;i++)n+=!!vc[i].gate;return n;}
static void init(void){engine_init();for(unsigned t=0;t<4;t++){trk_load(t,0);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_ARP_MODE]=A_OFF;live_p[t].v[P_GLIDE]=0;set.mix_lvl[t]=100;}set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();pat_init(&pat);step_held=-1;seq_trk=0;layer=L_NONE;keyboard_cfg_t k=pat.keyboard;k.mode=KB_LAYER;keyboard_changed(k);}
int main(void){
 init();
 /* A FIFO flooded before an ISR still delivers every release; no held notes. */
 for(unsigned n=24;n<96;n++)note_in(1,n,100);for(unsigned n=24;n<96;n++)note_in(0,n,0);render(3);assert(!gates());for(unsigned t=0;t<4;t++)assert(!nheld[t]);
 /* Held notes with overlapping octave remaps share a reference, released last. */
 keyboard_cfg_t k=pat.keyboard;note_in(1,60,100);render(1);k.octave[0]=1;keyboard_changed(k);note_in(1,48,100);render(1);assert(kb_refs[0][60]==2);note_in(0,60,0);render(1);assert(kb_refs[0][60]==1);note_in(0,48,0);render(1);assert(!gates()&&!nheld[0]&&!nheld[1]);
 /* Deferred engine-switch keys keep combined gain metadata. */
 obv_drums_choose(1,0);render(3);obv_analog_choose(1);note_in(1,65,100);render(3);unsigned found=0;for(unsigned i=0;i<NV;i++)if(vc[i].gate&&vc[i].part==1){found++;assert(vc[i].key_combined&&!vc[i].seq_owned);}assert(found);note_in(0,65,0);render(1);assert(!gates());
 /* Deferred same-pitch sequence/live notes occupy separate pending entries. */
 obv_drums_choose(1,0);render(3);obv_analog_choose(1);seq_voice_trigger(1,65,100);note_in(1,65,100);render(3);unsigned keys_count=0,sequenced=0;for(unsigned i=0;i<NV;i++)if(vc[i].gate&&vc[i].part==1){keys_count+=!vc[i].seq_owned;sequenced+=vc[i].seq_owned;}assert(keys_count&&sequenced);seq_voice_release(1,65,1);note_in(0,65,0);render(1);assert(!gates());
 /* Two arpeggiators share keyboard input; every generated note retains gain ownership. */
 live_p[0].v[P_ARP_MODE]=live_p[1].v[P_ARP_MODE]=A_UP;ctl_sustain=1;note_in(1,60,100);note_in(1,64,100);render(80);assert(nheld[0]==2&&nheld[1]==2);for(unsigned i=0;i<NV;i++)if(vc[i].gate)assert(vc[i].key_combined&&!vc[i].seq_owned);note_in(0,60,0);note_in(0,64,0);ctl_sustain=0;render(10);assert(!nheld[0]&&!nheld[1]&&!gates());live_p[0].v[P_ARP_MODE]=live_p[1].v[P_ARP_MODE]=A_OFF;
 /* A held REC key from the outgoing pattern may not add ties to the incoming one. */
 pat_init(&pat);keyboard_cfg_t combo=pat.keyboard;combo.mode=KB_LAYER;keyboard_changed(combo);rec=REC_READY;seq_trk=0;fm1_ms=100;note_in(1,60,100);render(1);assert(rec_open_part[0][60].t);pat_q=pat;for(unsigned t=0;t<4;t++)for(unsigned n=0;n<256;n++)step_clear(&pat_q.st[t][n]);ctl_pat_swap=1;render(500);assert(!ctl_pat_swap);pattern_t incoming=pat;fm1_ms+=400;note_in(0,60,0);assert(!memcmp(&pat,&incoming,sizeof pat));rec=0;ctl_seq_run=0;render(2);
 /* Sequencer timing, all drum lanes, four sends and live layers coexist. */
 pat_init(&pat);keyboard_changed(combo);obv_drums_choose(2,1);render(3);for(unsigned n=0;n<16;n++){pat.st[0][n].note[0]=48+n%5;pat.st[1][n].note[0]=60+n%5;drum_pattern_set(&pat,2,n,n%4?8:255);}ctl_mix_cho[0]=ctl_mix_dly[0]=ctl_mix_rev[0]=ctl_mix_drv[0]=127;ctl_seq_run=ctl_seq_restart=1;
 kb_edit_row=1;keyboard_cfg_t second=combo;second.mode=KB_SPLIT;second.split=73;second.level[0]=40;keyboard_changed(second);kb_edit_row=0;
 uint64_t energy=0;for(unsigned b=0;b<1500;b++){if(b%100==0)keyboard_pad((b/100)%2?15:7,100);if(b%70==0)note_in(1,72,100);if(b%70==40)note_in(0,72,0);if(b%130==0)ctl_seq_transpose=(int32_t)(b%7)-3;render(1);for(unsigned j=0;j<HALF_FRAMES*2;j++)energy+=(int64_t)audio[j]*audio[j];}note_in(0,72,0);ctl_seq_run=0;render(8);assert(energy>1000000&&!gates());
 for(unsigned t=0;t<4;t++)assert(!host_write_count[t]);
 puts("PASS: queue overflow release safety, octave overlap references, deferred combined metadata, dual arp/sustain, held REC across pattern switch, mixed sequencer/DRUM-X/two-row switching/transpose/all sends; no automatic writes. Host execution, not physical CPU validation.");
}
