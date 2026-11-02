/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <stdio.h>
#include <string.h>
static unsigned checks;
#define CHECK(c) do{checks++;if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);return 1;}}while(0)
static void pad(unsigned p,unsigned vel){host_qput(0,(int32_t)(p<<8|vel));smk_pads(KEY0,&dirty_steps);}
static void tap(unsigned p){pad(p,95);fm1_ms+=50;pad(p,0);}
static void setup(void) {
    engine_init();memset(&set,0,sizeof set);memset(&pat,0,sizeof pat);memset(psect,0,sizeof psect);
    memset(host_perf,0,sizeof host_perf);memset(host_perf_len,0,sizeof host_perf_len);
    memset(pf_role,0,sizeof pf_role);memset(pf_fired,0,sizeof pf_fired);memset(pf_flash,0,sizeof pf_flash);
    memset(smk_live,0,sizeof smk_live);pf_pending=0;
    host_qr[0]=host_qr[1]=host_qw[0]=host_qw[1]=0;
    for(unsigned t=0;t<4;t++){trk_load(t,t*6);pat.trk[t].len=16;set.mix_lvl[t]=100;}
    set.mix_dly[0]=set.mix_rev[0]=127;fx_sync();mix_apply();
    seq_trk=0;rec=0;layer=0;page=PG_PERFORM;step_held=-1;ctl_seq_run=0;fm1_ms=3000;
    pf_init();
}
int main(void){
    setup();CHECK(!pf_seq_bank());CHECK(NTRK==4&&NPROG==99);
    for(unsigned t=0;t<4;t++){tap(t);CHECK(seq_trk==t);CHECK(!seq_step(t,0)->note[0]);}
    uint32_t original=trk_prog[3],a=trk_prog[0];tap(5);CHECK(trk_prog[3]==(original+1)%99&&trk_prog[0]==a);
    tap(4);CHECK(trk_prog[3]==original);trk_load(3,0);tap(4);CHECK(trk_prog[3]==98);tap(5);CHECK(trk_prog[3]==0);
    seq_trk=1;set.mix_dly[1]=64;mix_apply();unsigned global=live_p[0].v[P_DLY_MIX];tap(6);
    CHECK(set.mix_dly[1]==0&&set.mix_dly[0]==127&&live_p[0].v[P_DLY_MIX]==global);
    tap(6);CHECK(set.mix_dly[1]==64&&ctl_mix_dly[1]==64&&set.mix_dly[0]==127);
    tap(7);CHECK(set.mix_rev[1]==127&&set.mix_rev[0]==127);tap(7);CHECK(set.mix_rev[1]==0);
    live_p[0].v[P_DLY_MIX]=0;set.mix_dly[1]=0;tap(6);CHECK(live_p[0].v[P_DLY_MIX]==48&&set.mix_dly[1]==64);
    fm1_ms=4000;uint16_t bright=pf_led(6);
    for(unsigned ms=0;ms<2000;ms+=17){fm1_ms=4000+ms;CHECK(pf_led(6)==bright);}
    tap(6);CHECK(pf_led(6)!=bright);tap(6);CHECK(pf_led(6)==bright);
    CHECK(C_TRKC[0]==C_RED);CHECK(text_w(&FONT_S,"FAV8")<=50);
    memset(pf_flash,0,sizeof pf_flash);smk_leds();uint32_t frame[16];memcpy(frame,padled_rgb,sizeof frame);
    for(unsigned ms=0;ms<2000;ms+=40){fm1_ms+=40;smk_leds();CHECK(!memcmp(frame,padled_rgb,sizeof frame));}
    seq_trk=2;live_p[2].v[P_CUTOFF]=77;trk_edited[2]=1;set.mix_dly[2]=45;unsigned prog=trk_prog[2];
    pad(8,90);fm1_ms+=699;pf_hold_poll();CHECK(!(pf_store.used&1));
    live_p[2].v[P_CUTOFF]=44;seq_trk=3;fm1_ms++;pf_hold_poll();
    CHECK(pf_store.used&1);CHECK(pf_store.slot[0].patch.v[P_CUTOFF]==77&&pf_store.slot[0].prog==prog&&pf_store.slot[0].edited==1);
    CHECK(live_p[2].v[P_CUTOFF]==44);pad(8,0);CHECK(live_p[2].v[P_CUTOFF]==44);
    seq_trk=0;global=live_p[0].v[P_DLY_MIX];tap(8);CHECK(live_p[0].v[P_CUTOFF]==77&&trk_prog[0]==prog);
    CHECK(live_p[0].v[P_DLY_MIX]==global&&set.mix_dly[0]==45&&live_p[2].v[P_CUTOFF]==44);
    unsigned before=trk_prog[0];tap(15);CHECK(trk_prog[0]==before&&!(pf_store.used>>7&1));
    for(unsigned i=1;i<8;i++) {seq_trk=i%4;live_p[seq_trk].v[P_CUTOFF]=(uint8_t)(20+i);pad(8+i,90);fm1_ms+=700;pad(8+i,0);CHECK(pf_store.slot[i].patch.v[P_CUTOFF]==20+i);}
    CHECK(pf_store.used==255&&pf_valid(&pf_store));
    /* Deferred saves never run during the sequencer or held voices. */
    ctl_seq_run=1;fm1_ms+=3000;pf_save_poll();CHECK(pf_dirty&&host_perf_len[0]==0);ctl_seq_run=0;
    nq_w=nq_r=0;for(unsigned i=0;i<NV;i++)vc[i].ae.stage=E_IDLE;dly_active=rev_active=0;
    pf_save_poll();CHECK(!pf_dirty&&!pf_pending&&host_perf_len[0]==sizeof pf_store);
    unsigned generation=pf_store.generation;pf_store.slot[0].patch.v[P_CUTOFF]=55;pf_changed();pf_manual_write=1;fm1_ms+=3000;pf_save_poll();
    CHECK(!pf_dirty&&pf_saved_bank==1&&pf_store.generation==generation+1);
    memset(&pf_store,0,sizeof pf_store);pf_init();CHECK(pf_store.slot[0].patch.v[P_CUTOFF]==55);
    unsigned gen=pf_store.generation;
    pf_store.slot[0].patch.v[P_CUTOFF]=56;pf_changed();pf_manual_write=1;fm1_ms+=3000;host_perf_fail=1;pf_save_poll();
    CHECK(pf_dirty&&pf_store.generation==gen);pf_init();CHECK(pf_store.slot[0].patch.v[P_CUTOFF]==55);
    /* A damaged latest record falls back to the previous valid bank. */
    memset(host_perf[1],0,4);pf_init();CHECK(pf_store.slot[0].patch.v[P_CUTOFF]==77);
    /* Manual bank, sequence page and REC automatically select sequencer; boot remains performance. */
    page=PG_PERFORM;rec=0;pf_manual_seq=0;fm1_in.buttons=1u<<BTN_KNOB_BANK;
    pf_bank_poll(1u<<BTN_KNOB_BANK,0);fm1_ms+=100;fm1_in.buttons=0;pf_bank_poll(0,1u<<BTN_KNOB_BANK);CHECK(pf_seq_bank());
    pf_manual_seq=0;page=PG_SEQ;CHECK(pf_seq_bank());page=PG_PERFORM;rec=REC_READY;CHECK(pf_seq_bank());rec=0;CHECK(!pf_seq_bank());
    /* A held REC chord releases all notes even after leaving REC and changing banks. */
    seq_trk=0;rec=REC_READY;pen_n=3;pen_chord[0]=60;pen_chord[1]=64;pen_chord[2]=67;nq_w=nq_r=0;
    pad(3,90);CHECK(nq_w==3);rec=0;pad(3,0);CHECK(nq_w==6);for(unsigned i=3;i<6;i++)CHECK(!nq[i].type);
    /* Favourites captured as Performance never turn into sequence steps on release. */
    memset(pf_role,0,sizeof pf_role);page=PG_PERFORM;rec=0;pf_manual_seq=0;seq_trk=0;
    pad(9,90);page=PG_SEQ;fm1_ms+=50;pad(9,0);CHECK(pf_role[9]==0);
    /* Long bank gesture opens diagnostics without toggling the pad bank. */
    page=PG_PERFORM;pf_manual_seq=0;fm1_in.buttons=1u<<BTN_KNOB_BANK;
    pf_bank_poll(1u<<BTN_KNOB_BANK,0);fm1_ms+=700;pf_bank_poll(0,0);CHECK(pf_diag);
    fm1_in.buttons=0;pf_bank_poll(0,1u<<BTN_KNOB_BANK);CHECK(!pf_manual_seq&&!pf_diag);
    /* Track pad colours match the UI; delay/reverb controls differ. */
    seq_trk=0;memset(pf_flash,0,sizeof pf_flash);CHECK(pf_led(0)==smk_dim(C_TRKC[0],240));CHECK(pf_led(1)==smk_dim(C_TRKC[1],18));
    CHECK(pf_led(4)!=pf_led(5));
    pf_pending=0;pf_flash[8]=0;pf_store.used|=1;seq_trk=0;pf_recall(0,0);
    CHECK(pf_matches(0,0));CHECK(perf_header_slot()==1);CHECK(pf_led(8)==smk_dim(C_RED,PF_SLOT_SELECTED));uint16_t selected=pf_led(8);live_p[0].v[P_CUTOFF]^=1;
    CHECK(!pf_matches(0,0)&&pf_led(8)!=selected);CHECK(perf_header_slot()==0);
    for(unsigned i=0;i<8;i++){pf_recall(i,0);CHECK(perf_header_slot()==i+1);}
    draw_header();write_ppm_as("build/host/pf2-favourite-header.ppm");
    tap(5);CHECK(perf_header_slot()==0);draw_header();write_ppm_as("build/host/pf2-program-header.ppm");
    pf_manual_seq=1;pen_n=1;pen_chord[0]=60;layer=0;step_held=-1;pad(2,80);
    CHECK(smk_pads_held>>2&1);pf_manual_seq=0;pad(2,0);CHECK(!(smk_pads_held>>2&1)&&step_held==-1);
    printf("PERFORMANCE: %u checks passed\n",checks);return 0;
}
