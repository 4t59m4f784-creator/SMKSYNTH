/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}
static unsigned gates(void){unsigned n=0;for(unsigned i=0;i<NV;i++)n+=!!vc[i].gate;return n;}
static void pedal(unsigned raw,unsigned ms){fm1_in.pedal=raw;while(ms--){fm1_ms++;smk_sustain_poll();smk_midi_controls_poll();}}
static void drain(void){for(unsigned i=0;i<64;i++)smk_midi_controls_poll();}
static void midi(unsigned cc,unsigned value){mq[mq_w%64][0]=0xB0;mq[mq_w%64][1]=cc;mq[mq_w++%64][2]=value;midi_in(0);}
static void init(unsigned rest){
 fx_used=0;engine_init();pf_init();pat_init(&pat);memset(&set,0,sizeof set);set.tempo=120;set.m_vol=100;
 for(unsigned t=0;t<NTRK;t++){trk_load(t,0);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_ARP_MODE]=A_OFF;}
 seq_trk=0;layer=L_NONE;rec=0;step_held=-1;fm1_in.frames=100;fm1_ms=100;
 memset(&smk_pedal,0,sizeof smk_pedal);midi_sustain=ctl_sustain=0;
 smk_cc_pending=smk_panic_tx=host_midi_count=host_midi_blocked=0;
 pedal(rest,120);assert(smk_pedal.ready&&!ctl_sustain);
}
int main(void){
 for(unsigned polarity=0;polarity<2;polarity++){
  unsigned rest=polarity?1000:0,pressed=1000-rest;init(rest);
  /* Debounce is time based, not how frequently LCD yields. */
  fm1_in.pedal=pressed;for(unsigned i=0;i<10000;i++)smk_sustain_poll();assert(!ctl_sustain);
  pedal(pressed,5);pedal(rest,20);assert(!ctl_sustain);
  pedal(pressed,30);drain();assert(ctl_sustain&&host_midi_count==4);
  for(unsigned c=0;c<4;c++)assert(host_midi_sent[c][0]==(0xB0|c)&&host_midi_sent[c][1]==64&&host_midi_sent[c][2]==127);
  keyboard_cfg_t k=pat.keyboard;k.mode=KB_LAYER;keyboard_changed(k);
  note_in(1,60,100);render();note_in(0,60,0);render();assert(gates()&&nheld[0]&&nheld[1]);
  /* Physical release must not cancel a MIDI-held pedal and vice versa. */
  midi(64,127);pedal(rest,30);render();assert(ctl_sustain&&gates());midi(64,0);render();assert(!ctl_sustain&&!gates());
  pedal(pressed,30);midi(64,127);midi(64,0);assert(ctl_sustain);
  /* Panic discards a queued onset, stops transport/record/HOLD, clears routes. */
  note_in(1,64,100);ctl_seq_run=1;rec=REC_ON;hold=ctl_hold=1;
  smk_panel_panic();render();assert(!gates()&&!ctl_sustain&&!ctl_hold&&!rec&&!ctl_seq_run&&nq_r==nq_w);
  for(unsigned t=0;t<NTRK;t++){assert(!nheld[t]);for(unsigned n=0;n<128;n++)assert(!kb_refs[t][n]);}
  host_midi_blocked=1;unsigned count=host_midi_count;drain();assert(smk_panic_tx==48&&host_midi_count==count);
  host_midi_blocked=0;drain();assert(!smk_panic_tx&&host_midi_count==count+48);
  for(unsigned i=0;i<48;i++){unsigned j=(count+i)%256;assert(host_midi_sent[j][0]==(0xB0|i/3));assert(host_midi_sent[j][1]==(i%3==0?64:i%3==1?123:120)&&!host_midi_sent[j][2]);}
  pedal(pressed,100);assert(!ctl_sustain);pedal(rest,40);pedal(pressed,40);assert(ctl_sustain);
  pedal(rest,40);note_in(0,64,0);note_in(1,67,100);render();assert(gates());note_in(0,67,0);render();assert(!gates());
 }
 /* The chord is consumed, including releases, then PLAY works normally. */
 unsigned pair=(1u<<BTN_GLO)|(1u<<BTN_PLAY),rel=0;
 fm1_in.buttons=pair;unsigned b=smk_panic_buttons(pair,&rel);assert(!b&&smk_panic_tx==48);
 fm1_in.buttons=0;rel=pair;assert(!smk_panic_buttons(0,&rel)&&!rel);
 b=smk_panic_buttons(1u<<BTN_PLAY,&rel);assert(b==(1u<<BTN_PLAY));
 puts("PASS: both pedal polarities, debounce, LAYER sustain, independent MIDI ownership, hard panic, queued onset discard, all 16 output channels, USB backpressure, pedal re-arm, panel chord and recovery. Host tests; physical validation pending.");
}
