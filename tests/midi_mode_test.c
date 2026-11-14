/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void drain(void){for(unsigned i=0;i<100;i++){fm1_ms++;mm_service();}}
static void step(void){fm1_ms++;mm_input();mm_wheels_poll();mm_service();}
static void button(unsigned b){host_edges=1u<<b;fm1_in.buttons=1u<<b;step();host_rel=1u<<b;fm1_in.buttons=0;step();}
static void key(unsigned k,unsigned down){if(down)fm1_in.keys|=(uint64_t)1<<k;else fm1_in.keys&=~((uint64_t)1<<k);fm1_in.key_vel[k]=101;step();}
static void pad(unsigned p,unsigned v){fm1_in.pad_st[p]=v?128:0;host_qput(0,p<<8|v);step();}
static void enc(unsigned k,int e){host_enc[k]=e;step();}
static void init(void){
 fx_used=0;engine_init();pf_init();pat_init(&pat);memset(&set,0,sizeof set);set.tempo=120;
 for(unsigned t=0;t<4;t++)trk_load(t,0);
 layer=rec=0;step_held=-1;fm1_in.frames=400;fm1_ms=1000;fm1_in.keys=fm1_in.notes=fm1_in.buttons=0;memset(fm1_in.pad_st,0,16);
 fm1_in.wheel[0]=535;fm1_in.wheel[1]=620;memset(&smk_pedal,0,sizeof smk_pedal);fm1_in.pedal=0;
 host_midi_count=host_midi_blocked=0;host_edges=host_rel=0;memset(host_mm_len,0,sizeof host_mm_len);
 mm_boot();mm_enter();drain();for(unsigned i=0;i<200;i++)step();drain();host_midi_count=0;
}
static void msg(unsigned idx,unsigned st,unsigned a,unsigned b){idx%=256;assert(host_midi_sent[idx][0]==st&&host_midi_sent[idx][1]==a&&host_midi_sent[idx][2]==b);}
int main(void){
 init();assert(mm_on&&!ctl_seq_run&&!rec&&engine_quiet());
 /* Complete physical keyboard, velocity and original pitch after octave edit. */
 key(0,1);key(36,1);drain();msg(0,0x90,48,101);msg(1,0x90,84,101);
 button(BTN_OCT_UP);key(0,0);key(36,0);drain();msg(2,0x80,48,0);msg(3,0x80,84,0);
 host_midi_count=0;pad(8,113);button(BTN_KNOB_BANK);button(BTN_FADER_BANK);pad(8,0);drain();msg(0,0x99,36,113);msg(1,0x89,36,0);
 /* Two sources share a pitch/channel and release only the final owner. */
 mm_pbank=0;mm_profile()->map[MM_PAD+8]=(mm_entry_t){MM_NOTE,60,1};mm_oct=0;host_midi_count=0;
 key(12,1);pad(8,100);key(12,0);drain();assert(host_midi_count==1);pad(8,0);drain();assert(host_midi_count==2);msg(1,0x80,60,0);
 /* Controller CCs coalesce under backpressure; note release goes first. */
 host_midi_count=0;key(12,1);drain();host_midi_blocked=1;
 for(unsigned n=0;n<1000;n++)mm_cv(0,(int)(n%128),0);
 key(12,0);assert(mm_key_sent[12]&&!mm_key_target[12]);host_midi_blocked=0;drain();msg(1,0x80,60,0);msg(2,0xB0,70,103);
 mm_profile()->relative=1;host_midi_count=0;mm_cv(0,170,1);drain();assert(host_midi_count==3);msg(0,0xB0,70,127);msg(1,0xB0,70,127);msg(2,0xB0,70,108);mm_profile()->relative=0;
 /* Editor: physical pad pick, typed mapping, per-pad saved palette. */
 button(BTN_EDIT);assert(mm_edit&&mm_pick);pad(3,100);pad(3,0);assert(!mm_pick&&mm_sel==MM_PAD+3);
 enc(3,1);assert(mm_profile()->color[3]==1);mm_edit=0;mm_leds();mm_edit=1;assert(padled_rgb[3]==((uint32_t)(40*80/255)<<16|80u<<8|(48*80/255)));
 unsigned original=mm_profile()->map[27].num;enc(1,2);assert(mm_profile()->map[27].num==original+2);
 mm_save();assert(!mm_dirty&&mm_data.seq==1);unsigned first=mm_slot;
 mm_profile()->color[3]=8;mm_dirty=1;host_mm_fail=1;mm_save();assert(mm_data.seq==1&&mm_slot==first&&mm_dirty);
 mm_on=0;mm_boot();assert(mm_profile()->color[3]==1);
 mm_profile()->color[3]=8;mm_save();assert(mm_data.seq==2);mm_boot();assert(mm_profile()->color[3]==8);
 host_mm[mm_slot][8]=255;mm_boot();assert(mm_profile()->color[3]==1); /* corrupt latest falls back */
 /* Four maps and channels stay independent. Held key blocked across changes. */
 mm_enter();drain();mm_setting_turn(1,1);assert(mm_data.selected==1&&mm_profile()->color[3]==0);mm_setting_turn(2,4);assert(mm_main_ch()==5);drain();key(0,1);drain();unsigned count=host_midi_count;mm_setting_turn(1,-1);assert(mm_main_ch()==1);drain();assert(host_midi_count==count+81);step();drain();assert(host_midi_count==count+83);key(0,0);key(0,1);drain();msg(host_midi_count-1,0x90,48,101);key(0,0);drain();
 /* Wheels follow keyboard routing and full 14-bit bend. */
 mm_profile()->map[MM_PITCH].ch=7;mm_wheel_prev[0]=-1;host_midi_count=0;fm1_in.wheel[0]=700;mm_wheels_poll();drain();msg(0,0xE0,127,127);
 mm_profile()->map[MM_MOD].ch=8;fm1_in.wheel[1]=485;mm_wheels_poll();drain();msg(host_midi_count-1,0xB0,1,127);
 /* Physical sustain in both polarities, main channel only, no local sustain. */
 for(unsigned polarity=0;polarity<2;polarity++){
  memset(&smk_pedal,0,sizeof smk_pedal);unsigned rest=polarity?1000:0;fm1_in.pedal=rest;for(unsigned n=0;n<130;n++)step();drain();host_midi_count=0;
  fm1_in.pedal=1000-rest;for(unsigned n=0;n<30;n++)step();drain();assert(!ctl_sustain);msg(0,0xB0,64,127);
  fm1_in.pedal=rest;for(unsigned n=0;n<30;n++)step();drain();msg(1,0xB0,64,0);
 }
 /* MIDI input cannot play local voices, and output silence is exact. */
 mq[mq_w%64][0]=0x90;mq[mq_w%64][1]=60;mq[mq_w++%64][2]=100;mm_discard_midi();assert(mq_r==mq_w);
 int32_t audio[512];memset(audio,123,sizeof audio);app_audio(audio,256);for(unsigned i=0;i<512;i++)assert(!audio[i]);
 /* Hard safety under a full wire queue; all channels reset, no lost fallback. */
 host_midi_blocked=1;for(unsigned n=0;n<129;n++)mm_queue_put(0xB0,2,n&127);assert(mm_panic_left==80&&mm_w==mm_r);host_midi_blocked=0;host_midi_count=0;drain();assert(host_midi_count==80);for(unsigned ch=0;ch<16;ch++){msg(ch*5,0xB0|ch,64,0);msg(ch*5+3,0xE0|ch,0,64);}
 mm_leave();assert(!mm_on&&page==PG_MIDI&&!ctl_seq_run);assert(fx_used==FX_MEM_N);
 mm_wait_release=0;mout_blocked=0;note_in(1,60,100);memset(audio,0,sizeof audio);app_audio(audio,256);assert(mout_w!=mout_r);note_in(0,60,0);
 puts("PASS: exclusive silence/local MIDI rejection; 37 keys, bank-safe pads, shared note ownership, bounded TX, relative deltas, four profiles with CRC record fallback, edit colours, full bend, per-control channels, both sustain polarities, overflow panic and safe return.");
}
