/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
static void timing_trace(int,uint32_t,uint32_t);
#define TRACE(c,t,n) timing_trace(c,t,n)
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static uint32_t trace_on,hits,last_hit,min_gap,max_gap,first_hit;
static void timing_trace(int c,uint32_t t,uint32_t n){
 (void)n;if(!trace_on||c!='S'||t)return;
 if(hits){uint32_t gap=eng_t-last_hit;if(gap<min_gap)min_gap=gap;if(gap>max_gap)max_gap=gap;}else first_hit=eng_t;
 last_hit=eng_t;hits++;
}
static int32_t audio[HALF_FRAMES*2];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}
static void reset(void){
 ctl_seq_run=0;ctl_panic=1;render();pat_init(&pat);seq_trk=0;layer=0;rec=0;step_held=-1;transpose_keys=0;ctl_seq_transpose=0;
 take_on=0;ctl_rec_mute=0;memset((void*)ctl_live_hold,0,sizeof ctl_live_hold);
 entry_reset();memset(seq_cur,0,sizeof seq_cur);notes_down=0;memset(kb_press,0,sizeof kb_press);memset(kb_refs,0,sizeof kb_refs);memset(panel_live_note,0,sizeof panel_live_note);
 for(unsigned t=0;t<NTRK;t++){trk_load(t,0);live_p[t].v[P_ARP_MODE]=A_OFF;patch_keyboard_octave_set(&live_p[t],0);}
 page=PG_SEQ;seq_view=SV_STEPS;
}
static void key(unsigned on,unsigned note){panel_note_in(on,note,on?101:0);}
int main(void){
 engine_init();pf_init();for(unsigned i=0;i<NPSECT;i++)psect[i].tag=PS_TAG;
 set.tempo=120;set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();
 /* All octave positions, four parts, chords and releases after OCT/focus changes. */
 for(unsigned t=0;t<4;t++)for(int oct=-2;oct<=2;oct++){
  reset();seq_trk=t;patch_keyboard_octave_set(&live_p[t],oct);pat.trk[t].transp=3;
  pat.st[t][0].note[0]=45;pat.st[t][0].nudge=93;
  key(1,60);key(1,64);assert(pat.st[t][0].note[0]==60+12*oct-3);assert(pat.st[t][0].note[1]==64+12*oct-3);assert(!pat.st[t][0].nudge);
  patch_keyboard_octave_set(&live_p[t],-oct);key(0,60);key(0,64);assert(seq_cur[t]==1&&!entry_n);
  seq_key_down(SMK_WK[4]);assert(pat.st[t][4].note[0]==60+12*oct-3&&pat.st[t][4].note[1]==64+12*oct-3);seq_key_up(SMK_WK[4]);
  /* Held pad step replacement shares routed pitch and removes inherited timing. */
  step_held=2;step_midi=0;pat.st[t][2].nudge=88;key(1,67);assert(pat.st[t][2].note[0]==67-12*oct-3&&!pat.st[t][2].nudge);key(0,67);step_held=-1;
  /* Live REC and first-note auto-start use exactly the same pitch. */
  rec=REC_READY;ctl_seq_run=0;key(1,60);assert(ctl_seq_run);assert(pat.st[t][0].note[2]==60-12*oct-3||pat.st[t][0].note[0]==60-12*oct-3||pat.st[t][0].note[1]==60-12*oct-3);
  seq_trk=(t+1)%4;key(0,60);assert(!kb_press[60].rec_on&&!seq_live(t,60-12*oct));
 }
 /* MIDI notes stay absolute, with no second application of panel OCT. */
 reset();panel_octave_set(2);note_in(1,60,100);note_in(0,60,0);assert(pat.st[0][0].note[0]==60);
 /* Layer and split: write only the selected part, using its octave routing. */
 reset();pat.keyboard=(keyboard_cfg_t){.mode=KB_LAYER,.part={0,1},.octave={1,-1},.level={100,100},.split=60};
 keyboard_pattern_gains(&pat);panel_octave_set(1);key(1,60);assert(pat.st[0][0].note[0]==84);key(0,60);
 seq_trk=1;key(1,60);assert(pat.st[1][0].note[0]==48);key(0,60);
 pat.keyboard.mode=KB_SPLIT;seq_trk=0;seq_cur[0]=3;key(1,72);key(0,72);assert(!pat.st[0][3].note[0]&&!entry_n);
 /* Arp REC uses the emitted, octave-correct note, without double transposition. */
 for(int oct=-2;oct<=2;oct++){
  reset();page=PG_PERFORM;panel_octave_set(oct);live_p[0].v[P_ARP_MODE]=1;live_p[0].v[P_ARP_OCT]=0;
  rec=REC_ON;ctl_seq_run=1;ctl_seq_restart=1;arp_qr=arp_qw=0;key(1,60);render();
  unsigned emitted=0;while(arp_qr!=arp_qw){arpev_t ev=arp_q[arp_qr++%AQ];
   if(ev.on){assert(ev.note==60+12*oct);emitted++;}
   rec_from_arp=1;rec_arp_q8=ev.q8;rec_note(ev.on,ev.note,ev.vel);rec_from_arp=0;
  }
  assert(emitted&&pat.st[0][0].note[0]==60+12*oct);key(0,60);rec_arp_q8=~0u;
 }
 /* Transpose remains enabled through every page, bank, focus and transport. */
 reset();page=PG_SEQ;seq_view=SV_TRANSPOSE;seq_view_turn(0,1);assert(transpose_keys);
 for(unsigned pg=0;pg<sizeof PAGE/sizeof PAGE[0];pg++){page=pg;obv_context_refresh();assert(transpose_keys);}
 for(unsigned bank=0;bank<5;bank++){pf_nav(bank);assert(transpose_keys);}
 page=PG_PERFORM;layer=0;key(1,50);assert(ctl_seq_transpose==2);key(0,50);assert(!entry_n&&!notes_down);
 pf_manual_seq=0;pf_seq_mode=0;draw_header();draw_page();draw_scope();write_ppm_as("build/dp2-performance.ppm");
 uint8_t rel=live_p[0].v[P_AR];host_enc[ENC_KNOB4]=-1;panel_encoders();assert(!transpose_keys&&live_p[0].v[P_AR]==rel&&ctl_seq_transpose==2);
 transpose_keys=1;key(1,55);transpose_keys_set(0);key(0,55);assert(!transpose_captured[55]&&!nheld[0]);
 /* Clock: 128 intervals per tempo/division, no accumulating drift or engine jitter. */
 for(unsigned tempo=40;tempo<=240;tempo++)for(unsigned div=0;div<NDIV;div++){
  reset();ctl_tempo=tempo;pat.trk[0].div=div;pat.trk[0].len=16;
  for(unsigned i=0;i<16;i++)pat.st[0][i].note[0]=60+i%12;
  for(unsigned t=1;t<4;t++)pat.trk[t].len=0;
  eng_t=0;seq_was=0;ctl_seq_restart=1;ctl_seq_run=1;hits=0;min_gap=~0u;max_gap=0;trace_on=1;
  uint32_t q=quarter_len(),dv=DIV_PER_QUARTER[div];
  while(hits<129){eng_t+=CTL;seq_tick();}
  trace_on=0;uint32_t expected=128u*q/dv,actual=last_hit-first_hit;
  assert(actual>=expected&&actual-expected<CTL);assert(max_gap-min_gap<=CTL);
 }
 printf("PASS: all four parts and five octaves, step/chord/held-pad/live-REC/autostart, MIDI absolute, split/layer routing, inherited nudge removed, persistent transpose + K4 and release ownership; 201 tempos x %u divisions x 128 intervals, phase error <16 frames (0.363 ms). Physical CPU not measured.\n",NDIV);
}
