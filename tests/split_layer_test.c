/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(unsigned n){while(n--){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}}
static unsigned gates(unsigned p,unsigned seq){unsigned n=0;for(unsigned i=0;i<NV;i++)n+=vc[i].gate&&vc[i].part==p&&vc[i].seq_owned==seq;return n;}
static void off(void){for(unsigned n=0;n<128;n++)note_in(0,n,0);ctl_sustain=0;ctl_seq_run=0;ctl_panic=1;render(2);memset(sr,0,sizeof sr);}
static void init_parts(void){for(unsigned t=0;t<4;t++){trk_load(t,0);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_GLIDE]=0;live_p[t].v[P_ARP_MODE]=A_OFF;live_p[t].v[P_VINTAGE]=0;live_p[t].v[P_LFO_AMP]=0;set.mix_lvl[t]=100;}}
static void combo(unsigned mode){keyboard_cfg_t k;keyboard_defaults(&k);k.mode=mode;keyboard_changed(k);}
static void pad(unsigned p,unsigned vel){uint32_t d=0;host_qput(0,(int32_t)(p<<8|vel));smk_pads(48,&d);}
int main(void){
 engine_init();for(unsigned i=0;i<NPSECT;i++)psect[i].tag=PS_TAG;init_parts();set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();pf_init();pat_init(&pat);step_held=-1;layer=L_NONE;seq_trk=0;
 /* Bank 4 only: two-pad chord chooses order, LAYER default; SINGLE is immediate. */
 page=PG_PERFORM;pf_manual_seq=3;assert(pf_bank()==3);pad(0,100);pad(2,100);pad(0,0);pad(2,0);assert(pat.keyboard.mode==KB_LAYER&&pat.keyboard.part[0]==0&&pat.keyboard.part[1]==2&&page==PG_KEYBOARD);pad(4,100);pad(4,0);assert(!pat.keyboard.mode);
 /* Real knobs have exclusive keyboard gains and ±2 octave limits. */
 combo(KB_LAYER);seq_trk=0;page=PG_KEYBOARD;kb_edit_oct=0;keyboard_turn(1,-60);assert(pat.keyboard.level[0]==40&&ctl_mix_lvl[0]==100);kb_edit_oct=1;keyboard_turn(2,1);assert(pat.keyboard.octave[1]==1);keyboard_turn(3,-1);assert(pat.keyboard.split==59);
 note_in(1,60,100);render(2);assert(gates(0,0)==1&&gates(1,0)==1);assert(held[0][0].note==60&&held[1][0].note==72);
 /* Release after focus, mode, octave and selected pair changes goes to original targets. */
 seq_trk=3;combo(KB_SINGLE);note_in(0,60,0);render(1);assert(!gates(0,0)&&!gates(1,0));
 combo(KB_SPLIT);note_in(1,59,100);note_in(1,60,100);render(1);assert(gates(0,0)==1&&gates(1,0)==1);note_in(0,59,0);note_in(0,60,0);render(1);assert(!gates(0,0)&&!gates(1,0));
 /* Sustain retains both until pedal up, with the existing per-part priority. */
 combo(KB_LAYER);ctl_sustain=1;note_in(1,64,110);render(1);note_in(0,64,0);render(1);assert(gates(0,0)&&gates(1,0));ctl_sustain=0;render(1);assert(!gates(0,0)&&!gates(1,0));
 /* A sequence and a live layer at exactly the same pitch have distinct offs/gain. */
 off();combo(KB_LAYER);seq_trk=0;seq_voice_trigger(0,60,100);note_in(1,60,100);render(2);assert(gates(0,1)==1&&gates(0,0)==1&&gates(1,0)==1);
 keyboard_cfg_t k=pat.keyboard;k.level[0]=0;keyboard_changed(k);render(1);unsigned seq_amp=0;for(unsigned i=0;i<NV;i++)if(vc[i].gate&&vc[i].part==0){if(vc[i].seq_owned)seq_amp=vc[i].amp1;else assert(vc[i].amp1==0);}assert(seq_amp>0&&ctl_mix_lvl[0]==100);
 ctl_seq_transpose=2;render(1);for(unsigned i=0;i<NV;i++)if(vc[i].gate&&vc[i].part==0)assert(ui_vnote[i]==(vc[i].seq_owned?62:60));
 sr[0].nsnd=1;sr[0].snd[0]=60;seq_release_t(0);assert(!gates(0,1)&&gates(0,0));note_in(0,60,0);render(1);assert(!gates(0,0)&&!gates(1,0));
 /* Live release cannot end the sequence in the opposite order either. */
 seq_voice_trigger(0,60,100);note_in(1,60,100);render(1);note_in(0,60,0);render(1);assert(gates(0,1)&&!gates(0,0));seq_voice_release(0,60,1);assert(!gates(0,1));ctl_seq_transpose=0;
 /* REC writes ONLY the selected member, including that member's audible octave. */
 off();pat_init(&pat);combo(KB_LAYER);k=pat.keyboard;k.octave[1]=1;keyboard_changed(k);seq_trk=1;rec=REC_READY;ctl_seq_run=0;fm1_ms=100;note_in(1,60,100);render(1);assert(gates(0,0)&&gates(1,0));assert(pat.st[1][0].note[0]==72&&!pat.st[0][0].note[0]);seq_trk=2;fm1_ms+=200;note_in(0,60,0);render(1);assert(!rec_open_part[1][72].t);rec=REC_OFF;ctl_seq_run=0;render(1);
 /* Legacy pattern and AD1 DRUM extension load SINGLE, with exact prefixes/scenes. */
 off();pat_init(&pat);pattern_t original=pat;snd_capture(0);psnd_t scene=psnd[0],readscene;
 assert(!slot_save(0,&pat,PAT_LEGACY_BYTES,&scene,sizeof scene));pattern_t loaded;memset(&loaded,0xCC,sizeof loaded);assert(!pattern_slot_load(0,&loaded,&readscene));assert(!loaded.keyboard.mode&&!memcmp(&loaded,&original,PAT_LEGACY_BYTES)&&!memcmp(&scene,&readscene,sizeof scene));
 drum_pattern_set(&pat,3,255,255);original=pat;assert(!slot_save(1,&pat,PAT_DRUM_BYTES,&scene,sizeof scene));assert(!pattern_slot_load(1,&loaded,&readscene));assert(!loaded.keyboard.mode&&!memcmp(&loaded,&original,PAT_DRUM_BYTES));
 /* The new extension round-trips; scene remains byte-identical and contains no config. */
 combo(KB_SPLIT);k=pat.keyboard;k.octave[0]=-2;k.level[1]=37;k.split=53;keyboard_changed(k);original=pat;
 assert(!pattern_slot_save(2,&pat,&scene));assert(!pattern_slot_load(2,&loaded,&readscene));assert(!memcmp(&loaded,&original,sizeof pat)&&!memcmp(&readscene,&scene,sizeof scene));
 assert(sizeof(psnd_t)==380u&&sizeof(pattern_t)==14476u);assert(pat.st[0][0].note[0]==original.st[0][0].note[0]);
 /* Pattern configuration is restored on the same queued downbeat, transpose stays global. */
 seq_trk=0;ctl_seq_transpose=-1;pat_init(&pat);ctl_seq_run=ctl_seq_restart=1;render(1);pat_go(2);render(500);assert(!ctl_pat_swap&&pat.keyboard.mode==KB_SPLIT&&pat.keyboard.split==53&&keyboard_gain[1]==37&&ctl_seq_transpose==-1);ctl_seq_run=0;render(1);
 /* Scenes/favourites change sounds, not the keyboard config. DRUM-X is rejected. */
 k=pat.keyboard;pf_snapshot(&pf_store.slot[0],0);pf_store.used=1;pf_recall(0,0);assert(!memcmp(&pat.keyboard,&k,sizeof k));
 obv_drums_choose(1,0);render(3);assert(!keyboard_active());pf_manual_seq=3;page=PG_KEYBOARD;pad(1,100);pad(1,0);assert(!memcmp(&pat.keyboard,&k,sizeof k));
 /* Dedicated drum pads bypass combinations and retain real-time recording. */
 init_parts();render(3);obv_drums_choose(2,0);render(3);combo(KB_LAYER);seq_trk=2;page=PG_DSOUND;pf_manual_seq=0;
 for(unsigned n=0;n<3;n++){pad(0,100);render(1);pad(0,0);render(1);}assert(drums_busy(2));assert(!gates(0,0)&&!gates(1,0));
 /* Combined voices keep the flag through mono handoff and engine fades. */
 off();init_parts();combo(KB_LAYER);live_p[0].v[P_MODE]=live_p[1].v[P_MODE]=M_MONO;note_in(1,60,100);note_in(1,64,100);render(1);note_in(0,64,0);render(1);for(unsigned i=0;i<NV;i++)if(vc[i].gate){assert(vc[i].note==60&&vc[i].key_combined);}note_in(0,60,0);render(1);assert(!gates(0,0)&&!gates(1,0));
 /* Over-range octaves skip notes; changing octaves with held keys cannot strand them. */
 off();init_parts();combo(KB_LAYER);k=pat.keyboard;k.octave[0]=2;k.octave[1]=-2;keyboard_changed(k);note_in(1,120,100);render(1);assert(!gates(0,0)&&gates(1,0));note_in(0,120,0);render(1);assert(!gates(1,0));
 puts("PASS: two-pad bank, SINGLE/SPLIT/LAYER, boundaries/octaves, exclusive live gain, same-pitch sequence isolation, sustain, REC selected-only, focus/setting releases, old/new patterns and scene bytes, queued restore, DRUM exclusion/hits, mono handoff, range safety.");
 page=PG_KEYBOARD;seq_trk=0;kb_edit_oct=0;draw_header();draw_page();draw_keyboard_scope();draw_hints();write_ppm_as("build/split-layer-levels.ppm");kb_edit_oct=1;draw_page();write_ppm_as("build/split-layer-octaves.ppm");
}
