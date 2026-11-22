/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void release_button(unsigned b){uint32_t rel=1u<<b;assert(!(su2_init_buttons(0,&rel)&(1u<<b)));assert(!(rel&(1u<<b)));}
static void press(unsigned b){uint32_t rel=0;assert(!(su2_init_buttons(1u<<b,&rel)&(1u<<b)));release_button(b);}
static void init_page(void){page=PG_INITIALIZE;layer=0;su2_init_seen=0;su2_init_stage=0;uint32_t rel=0;su2_init_buttons(0,&rel);}
static void snap(const char*name){char path[160];snprintf(path,sizeof path,"build/su2/%s.ppm",name);write_ppm_as(path);}
int main(){
 engine_init();pf_init();pat_init(&pat);mm_boot();set.tempo=110;set.m_vol=100;set.m_dj=64;seq_trk=0;fm1_ms=3000;
 const char*names[]={"WARM STR","SOFT PAD","DEEP BASS","DRUM"};
 for(unsigned t=0;t<NTRK;t++){patch_from_factory(&live_p[t],&FACT_INIT);memset(live_p[t].v+NPARAM,' ',PATCH_NAME);memcpy(live_p[t].v+NPARAM,names[t],strlen(names[t]));trk_prog[t]=22+t*10;set.mix_lvl[t]=100;}
 fx_sync();mix_apply();
 /* Real encoder -> immediate master controls, with neither pattern switch nor save. */
 page=PG_MIXER;mix_row=6;layer=0;memset(host_enc,0,sizeof host_enc);host_enc[0]=2;host_enc[1]=-2;host_enc[2]=2;host_enc[3]=2;panel_encoders();
 assert(set.m_vol==ctl_m_vol&&set.m_dj==ctl_m_dj&&set.m_drive==ctl_m_drive&&set.m_comp==ctl_m_comp);assert(ctl_m_comp&&ctl_m_drive&&ctl_m_dj!=64);
 /* All six actual mixer grids receive exactly their four part badges. */
 zm_zoom=1;for(unsigned row=0;row<7;row++){mix_row=row;draw_page();if(row<6)for(unsigned i=0;i<4;i++)assert(fb[(54+i/2*76)*240+4+i%2*120]==C_TRKC[i]);if(row==0)snap("mixer-levels");if(row==3)snap("send-delay");}
 /* Actual selected SINGLE part can differ from the remembered pair. */
 pf_nav(3);page=PG_KEYBOARD;seq_trk=2;zm_zoom=1;draw_page();snap("single-C");assert(fb[64*240+17]==C_TRKC[2]);
 pat.keyboard.mode=KB_LAYER;pat.keyboard.level[0]=90;pat.keyboard.level[1]=75;seq_trk=0;draw_page();snap("layer");
 pat.keyboard_alt=pat.keyboard;pat.keyboard_alt.mode=KB_SPLIT;pat.keyboard_alt.part[0]=2;pat.keyboard_alt.part[1]=0;kb_edit_row=1;draw_page();snap("split-edit2-play1");kb_edit_oct=1;draw_page();snap("split-octaves");kb_edit_oct=0;
 /* READY and progress are selected-part states, never inferred from content alone. */
 char status[32];end_mode=0;assert(!su2_end_status(status));endless_mark_tracks();end_mode=1;ctl_seq_run=0;assert(su2_end_status(status)==1&&!strcmp(status,"ENDLESS READY"));
 ctl_seq_run=1;ui_seq_pos[0]=4;assert(su2_end_status(status)==1&&!strcmp(status,"ENDLESS 5/16"));end_record=1;assert(su2_end_status(status)==2&&!strcmp(status,"ENDLESS REC"));end_record=0;
 for(unsigned th=0;th<2;th++)for(unsigned scope_view=0;scope_view<2;scope_view++){zm_zoom=th;zm_scope=scope_view;page=PG_PERFORM;layer=0;draw_page();if(!th&&!scope_view){draw_header();draw_scope();draw_leds();}char n[40];snprintf(n,sizeof n,"status-%u-%u",th,scope_view);snap(n);}zm_scope=0;
 /* REC overlay appears over a real edit page; release-before-threshold never clears. */
 page=PG_OSC1;zm_zoom=1;draw_page();su2_clear_t=0;rec_down=1;rec_down_t=fm1_ms-700;su2_ui_t=0;su2_ui_poll();assert(su2_overlay_was);snap("rec-hold");rec_down=0;fm1_ms+=60;su2_ui_poll();assert(!su2_overlay_was);
 trk_clear();assert(su2_clear_t&&su2_clear_part==0);fm1_ms+=60;su2_ui_poll();assert(su2_overlay_was);snap("rec-cleared");fm1_ms+=1200;su2_ui_poll();assert(!su2_overlay_was);
 end_record=rec=ctl_seq_run=0;end_mode=0;pat_queue=ctl_pat_swap=ctl_snd_swap=0;
 /* Review defaults to CANCEL. Ordinary PLAY action never escapes the page. */
 init_page();su2_init_target=0;seq_trk=1;patch_t before=live_p[1],other=live_p[2];mixset_t mx;mx_from_set(&mx);pat.st[1][0].note[0]=61;pattern_t saved=pat;
 unsigned writes[4];memcpy(writes,host_write_count,sizeof writes);press(BTN_PLAY);assert(su2_init_stage==1&&!su2_init_choice);press(BTN_PLAY);assert(!su2_init_stage&&!memcmp(&before,&live_p[1],80));
 press(BTN_PLAY);su2_init_turn(1);press(BTN_PLAY);assert(su2_init_stage==3&&trk_edited[1]);assert(!memcmp(&other,&live_p[2],80)&&!memcmp(&saved,&pat,sizeof pat));mixset_t now;mx_from_set(&now);assert(!memcmp(&now,&mx,sizeof mx));assert(!memcmp(writes,host_write_count,sizeof writes));assert(!memcmp(live_p[1].v+NPARAM,"INIT",4));
 assert(ctl_part_reset==2u);engine_events();assert(!ctl_part_reset);
 snap("init-patch-done");press(BTN_HOME);assert(su2_init_stage==0&&page==PG_INITIALIZE);press(BTN_HOME);assert(page==PG_GLOBAL);
 /* Pending pattern work refuses destructive confirmation. */
 init_page();su2_init_target=1;press(BTN_PLAY);su2_init_turn(1);pat_queue=2;press(BTN_PLAY);assert(su2_init_stage==1);pat_queue=0;press(BTN_HOME);
 /* Target changes invalidate review before an execution click. */
 su2_init_target=0;press(BTN_PLAY);su2_init_turn(1);seq_trk=2;press(BTN_PLAY);assert(su2_init_stage==1&&!su2_init_choice);press(BTN_HOME);
 /* Complete pattern init uses existing panic/ack, no flash write and 16 empty steps. */
 su2_init_target=1;press(BTN_PLAY);su2_init_turn(1);press(BTN_PLAY);assert(su2_init_stage==2);engine_events();fm1_ms+=60;su2_ui_poll();assert(su2_init_stage==3);
 assert(!end_mode&&!ctl_seq_run&&!rec&&pat_edited&&seq_trk==0);assert(!pat.keyboard.mode&&!pat.keyboard_alt.mode);
 for(unsigned t=0;t<NTRK;t++){assert(pat.trk[t].len==16);for(unsigned n=0;n<SEQ_STEPS;n++)assert(!pat.st[t][n].note[0]&&!pat.drum_mask[t][n]);for(unsigned n=0;n<NLANE;n++)assert(pat.lane[t][n].param==AUTO_NONE);assert(set.mix_lvl[t]==100&&!set.mix_pan[t]&&!set.mix_cho[t]&&!set.mix_dly[t]&&!set.mix_rev[t]&&!set.mix_drv[t]);assert(obv_patch_engine(&live_p[t])==(t==3?OBV_DRUMS:OBV_ANALOG));}
 assert(!live_p[0].v[P_CHO_MIX]&&!live_p[0].v[P_DLY_MIX]&&!live_p[0].v[P_REV_MIX]);assert(!memcmp(writes,host_write_count,sizeof writes));
 for(unsigned th=0;th<2;th++){zm_zoom=th;su2_init_stage=1;su2_init_choice=0;su2_init_draw();snap(th?"init-confirm-zoom":"init-confirm-obervave");}
 puts("PASS SU2: master real controls; six colored mixer pages; SINGLE actual-part identity; levels/octaves; Endless READY/play/REC; REC progress/abort/ack; init cancel/busy/target-change/execution; parts and manual flash preservation; INIT + basic DRUM-X, 16 empty steps per track.");
}
