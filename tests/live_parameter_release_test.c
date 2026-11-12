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

static unsigned gates(void){unsigned n=0;for(unsigned v=0;v<NV;v++)n+=!!vc[v].gate;return n;}
static void chord(unsigned on){for(unsigned i=0;i<3;i++)panel_note_in(on,60+4*i,on?100:0);render(2);}

static const unsigned order[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};
static void turn(unsigned knob,int delta){host_enc[ENC_KNOB1+knob]=delta;panel_encoders();render(1);}
static unsigned seq_gates(void){unsigned n=0;for(unsigned v=0;v<NV;v++)n+=vc[v].gate&&vc[v].seq_owned;return n;}
int main(void){
 /* The reported Glide edit alone is tested separately from MODE. */
 for(unsigned mode=0;mode<4;mode++)for(unsigned glide=0;glide<128;glide++){
  init(1);live_p[0].v[P_MODE]=mode;chord(1);set_param(P_GLIDE,glide);render(1);chord(0);assert(!gates());
 }
 /* Every mode transition, release order and part: old poly/dual voices must
  * still release after the current mode has become mono/unison. */
 for(unsigned part=0;part<4;part++)for(unsigned from=0;from<4;from++)for(unsigned to=0;to<4;to++)for(unsigned perm=0;perm<6;perm++){
  init(1);seq_trk=part;live_p[part].v[P_MODE]=from;chord(1);set_param(P_MODE,to);render(1);
  for(unsigned n=0;n<3;n++){panel_note_in(0,60+4*order[perm][n],0);render(1);}
  assert(!gates()&&!nheld[part]);
  for(unsigned n=0;n<128;n++)assert(!kb_press[n].count&&!kb_refs[part][n]);
 }
 /* Actual UI knobs, central hardware key path, Glide, mode, unison settings
  * and GLOBAL tempo/click/channel/tuning all retain releases. */
 init(1);page=PG_VOICE;layer=L_NONE;keys_prev=0;fm1_in.notes=(1u<<0)|(1u<<4)|(1u<<7);keys_in(0);render(1);
 turn(3,20);turn(0,1);turn(0,1);turn(1,1);turn(2,7);
 page=PG_GLOBAL;for(unsigned k=0;k<4;k++)turn(k,1);
 fm1_in.notes=0;keys_in(0);render(2);assert(!gates()&&!nheld[0]);
 /* Layer shares the pool but never loses either part's release ownership. */
 init(1);keyboard_cfg_t k=pat.keyboard;k.mode=KB_LAYER;keyboard_changed(k);chord(1);
 set_param_in(&live_p[0],P_MODE,M_MONO);set_param_in(&live_p[1],P_MODE,M_UNISON);render(1);chord(0);assert(!gates()&&!nheld[0]&&!nheld[1]);
 /* Sustain-up while one physical key remains held used the same early return. */
 init(1);chord(1);ctl_sustain=1;render(1);panel_note_in(0,60,0);panel_note_in(0,64,0);render(1);
 set_param(P_MODE,M_MONO);ctl_sustain=0;render(1);assert(gates()==1);for(unsigned v=0;v<NV;v++)if(vc[v].gate)assert(vc[v].note==68);
 panel_note_in(0,68,0);render(1);assert(!gates());
 /* Preserve the normal mono/unison last-key legato handoff. */
 for(unsigned mode=M_UNISON;mode<=M_MONO;mode++){
  init(1);live_p[0].v[P_MODE]=mode;panel_note_in(1,60,100);panel_note_in(1,64,100);render(1);
  panel_note_in(0,64,0);render(1);assert(gates());for(unsigned v=0;v<NV;v++)if(vc[v].gate)assert(vc[v].note==60);
  panel_note_in(0,60,0);render(1);assert(!gates());
 }
 /* A same-pitch sequencer note remains owned by the sequencer. */
 init(1);panel_note_in(1,60,100);panel_note_in(1,64,100);render(1);seq_voice_trigger(0,60,100);set_param(P_MODE,M_MONO);
 panel_note_in(0,60,0);panel_note_in(0,64,0);render(1);assert(seq_gates());seq_voice_release(0,60,1);render(1);assert(!gates());
 puts("PASS: Glide alone all 128 values in all modes; 384 mode/part/release-order combinations; actual UI and central keys across polyphony/global edits; Layer; sustain-up with a key still held; normal mono/unison legato; same-pitch sequencer ownership. The confirmed failure was POLY/DUAL to MONO/UNISON, not Glide alone. No hardware accessed.");
}
