/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include "../firmware/hal/smk_encoder_filter.h"
#include <assert.h>
#include <stddef.h>
static uint32_t d1,d2,d3;
static void poll(void){smk_faders(&d1,&d2,&d3);}
static void move(unsigned f,unsigned v){fm1_in.fader[f]=(uint8_t)v;poll();}
static void enc(unsigned k,int delta){host_enc[k]=delta;panel_encoders();}
static void reset(void){
 engine_init();pf_init();pat_init(&pat);memset(&set,0,sizeof set);set.click=CLICK_REC;set.tempo=90;
 set.fx_ok=FX_OK;set.m_vol=100;set.m_dj=64;set.m_ok=M_OK;set.rec_qt_ok=RQT_OK;set.dv_ok=DV_OK;
 for(unsigned t=0;t<NTRK;t++){patch_from_factory(&live_p[t],&FACT_INIT);trk_prog[t]=0;trk_edited[t]=0;}
 seq_trk=0;page=PG_PERFORM;layer=L_NONE;step_held=-1;mix_row=0;seq_view=0;rec=0;save_armed=0;
 preset_ui=pb_confirm=0;smk_bank=0;pf_manual_seq=0;pf_seq_mode=0;smk_master=724;fm1_in.frames=300;
 memset(smk_fader_owner,0,sizeof smk_fader_owner);memset(smk_fad_last,255,sizeof smk_fad_last);
 memset(fm1_in.fader,0,sizeof fm1_in.fader);memset(host_enc,0,sizeof host_enc);memset(host_write_count,0,sizeof host_write_count);
 poll();
}
static void encoder_filter(void){
 static const unsigned bad[]={0,1,3,2,1,3,2,0};int partial=0,out=0;
 for(unsigned n=1;n<sizeof bad/sizeof *bad;n++)out+=smk_encoder_delta(bad[n-1],bad[n],&partial);
 assert(out==0); /* stock decoder emitted -1 for this illegal two-bit trace */
 for(unsigned start=0;start<4;start++){
  static const unsigned cycle[]={0,1,3,2};unsigned at=0;while(cycle[at]!=start)at++;
  partial=0;out=0;for(unsigned n=0;n<400;n++)out+=smk_encoder_delta(cycle[(at+n)%4],cycle[(at+n+1)%4],&partial);assert(out==-100);
  partial=0;out=0;for(unsigned n=0;n<400;n++)out+=smk_encoder_delta(cycle[(at+400-n)%4],cycle[(at+399-n)%4],&partial);assert(out==100);
  partial=0;out=0;for(unsigned n=0;n<10000;n++){unsigned other=start^1u;out+=smk_encoder_delta(start,other,&partial);out+=smk_encoder_delta(other,start,&partial);}assert(!out&&!partial);
 }
 partial=3;assert(!smk_encoder_delta(0,3,&partial)&&!partial);
}
int main(void){
 _Static_assert(sizeof set==67,"legacy global record size");
 _Static_assert(sizeof(patch_t)==80,"legacy patch size");
 _Static_assert(sizeof(pf_store_t)==708,"legacy favorites size");
 encoder_filter();reset();assert(!smk_fader_jump());assert(offsetof(__typeof__(set),click)==7);
 /* Stationary controls, pages, banks and 10,000 UI polls never mutate Attack. */
 live_p[0].v[P_AA]=83;live_p[0].v[P_FA]=71;
 for(unsigned n=0;n<10000;n++)poll();assert(live_p[0].v[P_AA]==83&&live_p[0].v[P_FA]==71);
 for(unsigned pg=0;pg<NPAGE;pg++){page=pg;poll();for(unsigned f=0;f<4;f++)move(f,127);}
 assert(live_p[0].v[P_AA]==83&&live_p[0].v[P_FA]==71);
 /* Crossing in either direction, own writes remain caught, context edits rearm. */
 reset();live_p[0].v[P_CUTOFF]=64;poll();move(0,30);assert(live_p[0].v[P_CUTOFF]==64);move(0,64);assert(live_p[0].v[P_CUTOFF]==64&&smk_fader_owner[0].picked);move(0,80);assert(live_p[0].v[P_CUTOFF]==80);
 live_p[0].v[P_CUTOFF]=20;poll();move(0,60);assert(live_p[0].v[P_CUTOFF]==20);move(0,20);move(0,10);assert(live_p[0].v[P_CUTOFF]==10);
 page=PG_FENV;live_p[0].v[P_CUTOFF]=50;poll();move(0,30);assert(live_p[0].v[P_CUTOFF]==50);
 /* JUMP changes on movement only, and bank A never follows the displayed Attack. */
 smk_fader_mode_set(1);poll();move(0,55);assert(live_p[0].v[P_CUTOFF]==55);
 live_p[0].v[P_AA]=70;live_p[0].v[P_FA]=80;for(unsigned f=0;f<4;f++)move(f,90);
 assert(live_p[0].v[P_AA]==70&&live_p[0].v[P_FA]==80);
 /* All sends and physical master share CATCH; K8 changes invalidate F4 pickup. */
 reset();smk_bank=1;set.mix_cho[0]=64;set.mix_dly[0]=50;set.mix_rev[0]=30;poll();
 move(0,30);assert(set.mix_cho[0]==64);move(0,70);assert(set.mix_cho[0]==70);move(1,51);assert(set.mix_dly[0]==51);move(2,32);assert(set.mix_rev[0]==32);
 move(3,50);assert(smk_master==724);move(3,100);assert(smk_master==100*1023/127);
 smk_master=400;poll();move(3,80);assert(smk_master==400);move(3,20);assert(smk_master==20*1023/127);
 /* Mixer encoder, FX page and other parameter changes rearm target takeover. */
 page=PG_MIXER;mix_row=2;poll();enc(0,-1);assert(set.mix_cho[0]==68);poll();move(0,90);assert(set.mix_cho[0]==68);move(0,60);assert(set.mix_cho[0]==60);
 page=PG_CHORUS;set.mix_cho[0]=30;poll();move(0,80);assert(set.mix_cho[0]==30);
 /* Same-valued favourite/preset recalls, pattern swaps, part and pad-bank changes. */
 reset();live_p[0].v[P_CUTOFF]=64;poll();move(0,64);move(0,80);assert(smk_fader_owner[0].picked);
 pf_store.used=1;pf_store.slot[0].patch=live_p[0];pf_store.slot[0].prog=0;pf_store.slot[0].cho=64;fm1_in.fader[0]=90;pf_recall(0,0);poll();move(0,100);assert(live_p[0].v[P_CUTOFF]==80);
 move(0,75);assert(live_p[0].v[P_CUTOFF]==75);fm1_in.fader[0]=80;smk_controls_epoch++;poll();move(0,90);assert(live_p[0].v[P_CUTOFF]==75);
 ui_pat_swapped++;poll();move(0,100);assert(live_p[0].v[P_CUTOFF]==75);
 live_p[1].v[P_CUTOFF]=20;seq_trk=1;poll();move(0,90);assert(live_p[1].v[P_CUTOFF]==20);
 seq_trk=0;pf_manual_seq=3;poll();move(0,100);assert(live_p[0].v[P_CUTOFF]==75);
 /* Relative attack, mixer and shared FX start from their current value. */
 reset();page=PG_AENV;live_p[0].v[P_AA]=60;enc(0,1);assert(live_p[0].v[P_AA]==62);live_p[0].v[P_AA]=30;enc(0,-1);assert(live_p[0].v[P_AA]==28);
 page=PG_FENV;live_p[0].v[P_FA]=40;enc(0,1);assert(live_p[0].v[P_FA]==42);
 page=PG_MIXER;mix_row=0;set.mix_lvl[2]=80;enc(2,1);assert(set.mix_lvl[2]==81);
 page=PG_CHORUS;unsigned p=obv_page_param(page,0);unsigned was=live_p[0].v[p];enc(0,1);assert(live_p[0].v[p]==(was+2>127?127:was+2));
 /* Pending parameter delta must not be transferred by K6 or K7 navigation. */
 page=PG_AENV;live_p[1].v[P_AA]=65;host_enc[ENC_ALGORITHM]=1;host_enc[ENC_KNOB1]=1;panel_encoders();assert(seq_trk==1&&live_p[1].v[P_AA]==65);
 host_enc[ENC_PRESETS]=1;host_enc[ENC_KNOB1]=1;panel_encoders();patch_t expected;prog_get(trk_prog[1],&expected);assert(live_p[1].v[P_AA]==expected.v[P_AA]);
 /* MIDI and recorded automation remain valid independent parameter sources. */
 reset();cc_map_init();unsigned cc=0;while(cc<128&&cc_map[cc]!=P_AA+1u)cc++;assert(cc<128);
 mq[mq_w%64][0]=0xB0;mq[mq_w%64][1]=(uint8_t)cc;mq[mq_w%64][2]=77;mq_w++;midi_in(0);assert(live_p[0].v[P_AA]==77);
 pat.lane[0][0].param=P_AA;pat.lane[0][0].v[0]=45;seq_auto(0,0);assert(live_p[0].v[P_AA]==45);poll();assert(live_p[0].v[P_AA]==45);
 /* Global storage keeps exact record sizes and old click readers see 0..3. */
 for(unsigned click=0;click<4;click++)for(unsigned jump=0;jump<2;jump++){
  set.click=(uint8_t)click;smk_fader_mode_set(jump);assert(set.click%NCLICK==click&&!!smk_fader_jump()==jump);
  unsigned char saved[sizeof set];memcpy(saved,&set,sizeof set);assert(!settings_save(&set,sizeof set));memset(&set,0,sizeof set);assert(!settings_load(&set,sizeof set));assert(!memcmp(saved,&set,sizeof set));
  page=PG_GLOBAL;enc(1,1);assert(!!smk_fader_jump()==jump&&set.click%NCLICK==(click<3?click+1:3));
 }
 page=PG_CONTROLS;enc(0,-1);assert(smk_fader_jump());enc(0,1);assert(!smk_fader_jump());
 puts("PASS: illegal encoder trace, all Gray phases/directions and bounce; Attack isolation; JUMP/CATCH for all faders; mixer/FX/part/page/bank/recall/pattern rearm; relative encoders and context ownership; settings compatibility.");
}
