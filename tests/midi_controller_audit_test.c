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


static unsigned exact(unsigned begin,unsigned st,unsigned a,unsigned b){unsigned count=0;assert(host_midi_count-begin<=256);for(unsigned i=begin;i<host_midi_count;i++){uint8_t *m=host_midi_sent[i%256];count+=m[0]==st&&m[1]==a&&m[2]==b;}return count;}
static void wheels(unsigned pitch,unsigned mod){fm1_in.wheel[0]=pitch;fm1_in.wheel[1]=mod;smk_wheels();drain();}
static void pedal(unsigned raw,unsigned ms){fm1_in.pedal=raw;while(ms--){fm1_ms++;smk_sustain_poll();smk_midi_controls_poll();}}
int main(void){
 init(1);fm1_in.frames=400;wheels(535,620);
 /* Fixed rest and calibrated extremes: exact E0 14-bit and CC1 values. */
 for(unsigned base=0;base<=13;base++)for(unsigned part=0;part<4;part++){
  set.midi_ch=base;seq_trk=part;smk_midi_controls_poll();unsigned ch=(base?base-1:0)+part;
  wheels(535,620);unsigned begin=host_midi_count;wheels(700,485);
  assert(exact(begin,0xE0|ch,127,127)==1&&exact(begin,0xB0|ch,1,127)==1);
  begin=host_midi_count;wheels(370,620);assert(exact(begin,0xE0|ch,0,0)==1&&exact(begin,0xB0|ch,1,0)==1);
  begin=host_midi_count;wheels(535,620);assert(exact(begin,0xE0|ch,0,64)==1);
 }
 set.midi_ch=3;seq_trk=0;smk_midi_controls_poll();wheels(700,485);
 unsigned begin=host_midi_count;seq_trk=2;smk_wheels();drain();assert(exact(begin,0xE2,0,64)==1&&exact(begin,0xB2,1,0)==1&&exact(begin,0xE4,127,127)==1&&exact(begin,0xB4,1,127)==1);
 /* Changes while Panic blocks output must be resent afterward. */
 smk_panel_panic();begin=host_midi_count;fm1_in.wheel[0]=535;fm1_in.wheel[1]=620;smk_wheels();drain();smk_wheels();drain();
 assert(exact(begin,0xE4,0,64)==1&&exact(begin,0xB4,1,0)==1);
 render(1);
 /* Layer does not broadcast wheels: physical engine wheels target selected part. */
 keyboard_cfg_t k=pat.keyboard;k.mode=KB_LAYER;keyboard_changed(k);seq_trk=0;smk_wheels();drain();begin=host_midi_count;wheels(700,485);
 assert(exact(begin,0xE2,127,127)==1&&exact(begin,0xB2,1,127)==1&&!message(begin,0xE3,127)&&!message(begin,0xB3,1));
 /* MIDI queue full recovery also invalidates cached wheel values. */
 host_midi_blocked=1;for(unsigned i=0;i<MOUT_Q+1;i++)mout_emit(0x90,60,100);assert(mout_fault);smk_midi_controls_poll();
 fm1_in.wheel[0]=535;fm1_in.wheel[1]=620;smk_wheels();host_midi_blocked=0;drain();begin=host_midi_count;smk_wheels();drain();assert(exact(begin,0xE2,0,64)&&exact(begin,0xB2,1,0));
 /* Sustain reaches current four channels, not unrelated historical channels. */
 init(3);smk_pedal_channels=0;fm1_in.frames=400;pedal(0,120);
 mout_used_channels=0xFFFF;begin=host_midi_count;pedal(1000,30);drain();
 for(unsigned ch=0;ch<16;ch++)assert(exact(begin,0xB0|ch,64,127)==(ch>=2&&ch<6));
 begin=host_midi_count;seq_trk=3;smk_midi_controls_poll();assert(host_midi_count==begin); /* focus does not redirect pedal */
 /* Hold notes on old base, then change base while pedal stays down. */
 note_event_p(0,1,60,100);render(1);note_event_p(0,0,60,0);render(1);set.midi_ch=9;begin=host_midi_count;drain();
 for(unsigned ch=8;ch<12;ch++)assert(exact(begin,0xB0|ch,64,127)==1);
 begin=host_midi_count;pedal(0,30);drain();for(unsigned ch=2;ch<6;ch++)assert(exact(begin,0xB0|ch,64,0)==1);for(unsigned ch=8;ch<12;ch++)assert(exact(begin,0xB0|ch,64,0)==1);
 render(1);
 /* Release/repress during backpressure preserves OLD-channel sustain-off. */
 set.midi_ch=3;drain();pedal(1000,30);drain();set.midi_ch=9;drain();host_midi_blocked=1;pedal(0,30);render(1);pedal(1000,30);begin=host_midi_count;host_midi_blocked=0;drain();
 for(unsigned ch=2;ch<6;ch++)assert(exact(begin,0xB0|ch,64,0)==1&&!exact(begin,0xB0|ch,64,127));
 for(unsigned ch=8;ch<12;ch++)assert(exact(begin,0xB0|ch,64,127)==1);
 puts("PASS: exact 14-bit Pitch Bend and CC1 at all bases/OMNI and parts; old-channel reset; selected-only wheels in Layer; Panic/overflow resend; global four-part CC64, base changes with held notes, no stale historical destinations and rapid release/repress under backpressure. No hardware accessed.");
}
