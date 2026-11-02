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
 setup();memset(pf_flash,0,sizeof pf_flash);pf_pending=0;
 CHECK(pf_led(4)==smk_dim(C_RED,PF_NAV_IDLE));
 CHECK(pf_led(5)==smk_dim(PF_SLOT_BLUE,PF_NAV_IDLE));
 unsigned original=trk_prog[0];
 pad(5,95);CHECK(trk_prog[0]==(original+1u)%NPROG);
 CHECK(pf_led(5)==smk_dim(PF_SLOT_BLUE,PF_NAV_PRESSED));
 fm1_ms+=900;CHECK(pf_led(5)==smk_dim(PF_SLOT_BLUE,PF_NAV_PRESSED));
 pad(5,0);CHECK(pf_led(5)==smk_dim(PF_SLOT_BLUE,PF_NAV_IDLE));
 pad(4,95);CHECK(trk_prog[0]==original);CHECK(pf_led(4)==smk_dim(C_RED,PF_NAV_PRESSED));
 pad(4,0);CHECK(pf_led(4)==smk_dim(C_RED,PF_NAV_IDLE));
 live_p[0].v[P_DLY_MIX]=live_p[0].v[P_REV_MIX]=48;
 CHECK(pf_led(6)==smk_dim(C_CREAM,PF_FX_ACTIVE));
 CHECK(pf_led(7)==smk_dim(RGB(90,195,255),PF_FX_ACTIVE));
 uint16_t delay_led=pf_led(6),reverb_led=pf_led(7);fm1_ms+=600;
 CHECK(pf_led(6)==delay_led&&pf_led(7)==reverb_led);
 set.mix_dly[0]=set.mix_rev[0]=0;
 CHECK(pf_led(6)==smk_dim(C_CREAM,10));
 CHECK(pf_led(7)==smk_dim(RGB(90,195,255),10));
 set.mix_dly[0]=set.mix_rev[0]=127;
 CHECK(pf_led(8)==smk_dim(PF_SLOT_BLUE,PF_SLOT_EMPTY));
 pf_snapshot(&pf_store.slot[0],0);pf_store.slot[1]=pf_store.slot[0];pf_store.used=3;
 CHECK(pf_led(8)==smk_dim(PF_SLOT_BLUE,PF_SLOT_STORED));
 pf_recall(0,0);CHECK(pf_led(8)==smk_dim(C_RED,PF_SLOT_SELECTED));
 live_p[0].v[P_CUTOFF]^=1;CHECK(pf_led(8)==smk_dim(C_RED,PF_SLOT_EDITED));
 live_p[0].v[P_CUTOFF]^=1;CHECK(pf_led(8)==smk_dim(C_RED,PF_SLOT_SELECTED));
 pf_toggle(0);CHECK(pf_led(8)==smk_dim(C_RED,PF_SLOT_EDITED));pf_recall(0,0);
 seq_trk=1;CHECK(pf_led(8)==smk_dim(PF_SLOT_BLUE,PF_SLOT_STORED));seq_trk=0;
 pf_recall(1,0);CHECK(pf_led(8)==smk_dim(PF_SLOT_BLUE,PF_SLOT_STORED));
 set.pat=0;pat_used=0;pb_confirm=0;pb_write_target=0;pat_queue=0;
 CHECK(pb_led(0)==smk_dim(C_RED,PF_SLOT_EMPTY));
 pat_used=3;snd_capture(0);snd_dirty=0;pat_edited=0;
 CHECK(pb_led(0)==smk_dim(PF_SLOT_BLUE,PF_SLOT_SELECTED));
 CHECK(pb_led(1)==smk_dim(C_RED,PF_SLOT_STORED));CHECK(pb_led(2)==smk_dim(C_RED,PF_SLOT_EMPTY));
 live_p[0].v[P_CUTOFF]^=1;CHECK(pb_led(0)==smk_dim(PF_SLOT_BLUE,PF_SLOT_EDITED));
 snd_capture(0);CHECK(!snd_changed(0));CHECK(pb_led(0)==smk_dim(PF_SLOT_BLUE,PF_SLOT_EDITED));
 slot_saved(0,&pat);CHECK(pb_led(0)==smk_dim(PF_SLOT_BLUE,PF_SLOT_SELECTED));
 pat_edited=1;CHECK(pb_led(0)==smk_dim(PF_SLOT_BLUE,PF_SLOT_EDITED));pat_edited=0;
 set.mix_lvl[0]--;CHECK(pb_led(0)==smk_dim(PF_SLOT_BLUE,PF_SLOT_EDITED));
 for(unsigned i=0;i<1000;i++){fm1_ms+=40;for(unsigned p=0;p<16;p++){pf_led(p);pb_led(p);}}
 for(unsigned i=0;i<4;i++)CHECK(!host_write_count[i]);
 printf("SLOT LED STATES: %u checks passed; all four inverse states, dirty state survives RAM capture, momentary navigation, static dimmed effects, zero persistence calls.\n",checks);return 0;
}
