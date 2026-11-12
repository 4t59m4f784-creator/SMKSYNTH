/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(unsigned blocks){while(blocks--){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);smk_midi_controls_poll();}}
static void drain(void){for(unsigned i=0;i<100;i++)smk_midi_controls_poll();}
static unsigned message(unsigned begin,unsigned status,unsigned note){unsigned count=0;assert(host_midi_count-begin<=256);for(unsigned i=begin;i<host_midi_count;i++)count+=host_midi_sent[i%256][0]==status&&host_midi_sent[i%256][1]==note;return count;}
static void init(unsigned base){
 fx_used=0;memset(vc,0,sizeof vc);memset(pt,0,sizeof pt);for(unsigned t=0;t<4;t++)pt[t].p=&live_p[t];memset(arp,0,sizeof arp);memset(nheld,0,sizeof nheld);
 memset(sr,0,sizeof sr);memset(kb_press,0,sizeof kb_press);memset(kb_refs,0,sizeof kb_refs);for(unsigned t=0;t<4;t++)for(unsigned w=0;w<4;w++)keyboard_off[t][w]=0;
 nq_r=nq_w=0;sus_was=0;engine_init();pf_init();pat_init(&pat);memset(&set,0,sizeof set);set.tempo=120;set.m_vol=100;set.midi_ch=base;
 for(unsigned t=0;t<4;t++){trk_load(t,0);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_ARP_MODE]=A_OFF;}
 seq_trk=0;layer=0;step_held=-1;rec=0;ctl_seq_run=ctl_hold=ctl_sustain=ctl_panic=0;
 memset(&smk_pedal,0,sizeof smk_pedal);midi_sustain=0;smk_panic_tx=smk_cc_pending=0;
 midi_input_active=mout_suppress=host_midi_blocked=0;host_midi_count=0;
 mout_base=base?base-1:0;ctl_seq_transpose=0;render(3);drain();host_midi_count=0;
}
int main(void){
 for(unsigned base=0;base<=13;base++){
  init(base);unsigned ch=base?base-1:0;
  for(unsigned t=0;t<4;t++)note_event_p(t,1,60+t,100);render(2);drain();
  for(unsigned t=0;t<4;t++)assert(message(0,0x90|ch+t,60+t)==1);
  /* Focus and base edits cannot redirect the release of an existing note. */
  seq_trk=3;set.midi_ch=13;smk_midi_controls_poll();
  for(unsigned t=0;t<4;t++)note_event_p(t,0,60+t,0);render(2);drain();
  for(unsigned t=0;t<4;t++)assert(message(0,0x80|ch+t,60+t)==1);
 }
 init(3);keyboard_cfg_t k=pat.keyboard;k.mode=KB_LAYER;keyboard_changed(k);
 panel_note_in(1,60,100);render(2);drain();assert(message(0,0x92,60)==1&&message(0,0x93,60)==1);
 panel_octave_set(1);seq_trk=3;panel_note_in(0,60,0);render(2);drain();assert(message(0,0x82,60)==1&&message(0,0x83,60)==1);
 init(1);k=pat.keyboard;k.mode=KB_SPLIT;k.split=60;keyboard_changed(k);
 panel_note_in(1,48,100);panel_note_in(1,72,100);render(2);drain();assert(message(0,0x90,48)==1&&message(0,0x91,72)==1&&!message(0,0x90,72));
 init(1);
 /* Incoming notes are not raw MIDI Thru. */
 mq[mq_w%64][0]=0x90;mq[mq_w%64][1]=60;mq[mq_w++%64][2]=100;midi_in(0);render(2);drain();assert(!host_midi_count);
 mq[mq_w%64][0]=0x80;mq[mq_w%64][1]=60;mq[mq_w++%64][2]=0;midi_in(0);render(2);drain();assert(!host_midi_count&&!midi_input_active);
 /* Sequence output includes global transpose; changing it pairs old offs/new ons. */
 unsigned begin=host_midi_count;ctl_seq_transpose=2;seq_voice_trigger(2,60,90);drain();assert(message(begin,0x92,62)==1);
 ctl_seq_transpose=-1;render(1);drain();assert(message(begin,0x82,62)==1&&message(begin,0x92,59)==1);
 seq_voice_release(2,60,1);drain();assert(message(begin,0x82,59)==1);
 /* Simultaneous live/sequence same pitch: one source cannot release the other. */
 ctl_seq_transpose=0;begin=host_midi_count;ev_note_on(0,64,100);seq_voice_trigger(0,64,100);drain();assert(message(begin,0x90,64)==1);
 ev_note_off(0,64);drain();assert(!message(begin,0x80,64));seq_voice_release(0,64,1);drain();assert(message(begin,0x80,64)==1);
 /* ARP sends generated notes and gate releases rather than the source chord. */
 init(1);live_p[1].v[P_ARP_MODE]=A_UP;live_p[1].v[P_ARP_GATE]=50;note_event_p(1,1,60,100);render(25);drain();assert(message(0,0x91,60)>=1&&message(0,0x81,60)>=1);note_event_p(1,0,60,0);render(2);drain();
 /* Sequenced DRUM-X gets a finite external note lifetime. */
 init(1);obv_drums_choose(3,0);render(10);drain();host_midi_count=0;seq_voice_trigger(3,36,110);drain();assert(message(0,0x93,36)==1);render(8);drain();assert(message(0,0x83,36)==1);
 /* Overflow cannot silently strand outgoing notes: automatic all-channel panic. */
 init(1);host_midi_blocked=1;for(unsigned i=0;i<MOUT_Q+5;i++)mout_emit(0x90,60,100);assert(mout_fault&&mout_blocked);smk_midi_controls_poll();assert(smk_panic_tx==48);host_midi_blocked=0;drain();assert(!mout_blocked&&!smk_panic_tx&&host_midi_count==48);
 puts("PASS: all bases/OMNI + four part channels, focus/base/octave release ownership, Split/Layer, no raw input echo, sequence transpose, shared pitches, ARP gates, finite DRUM-X notes and saturated-queue panic recovery. Physical validation pending.");
}
