/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void release_button(unsigned b){uint32_t rel=1u<<b;assert(!(su2_init_buttons(0,&rel)&(1u<<b)));assert(!(rel&(1u<<b)));}
static void press(unsigned b){uint32_t rel=0;assert(!(su2_init_buttons(1u<<b,&rel)&(1u<<b)));release_button(b);}
static void init_page(void){page=PG_INITIALIZE;layer=0;su2_init_seen=0;su2_init_stage=0;uint32_t rel=0;su2_init_buttons(0,&rel);}
static void snap(const char*name){char path[160];snprintf(path,sizeof path,"build/su3/%s.ppm",name);write_ppm_as(path);}
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
 /* DRUM-X footer follows hits and retains percussion silhouettes, not voice meters. */
 patch_t keep=live_p[0];obv_drums_preset(&live_p[0],0);page=PG_PERFORM;zm_zoom=1;zm_scope=0;eng_t=100;drum_stamp[0][0]=100;draw_page();snap("drums-status");unsigned drum_pixels=0;for(unsigned y=203;y<213;y++)for(unsigned x=138;x<147;x++)drum_pixels+=fb[y*240+x]==C_RED;assert(drum_pixels>0);live_p[0]=keep;
 /* REC overlay appears over a real edit page; release-before-threshold never clears. */
 page=PG_OSC1;zm_zoom=1;draw_page();su2_clear_t=0;rec_down=1;rec_down_t=fm1_ms-700;su2_ui_t=0;su2_ui_poll();assert(su2_overlay_was);snap("rec-hold");rec_down=0;fm1_ms+=60;su2_ui_poll();assert(!su2_overlay_was);
 trk_clear();assert(su2_clear_t&&su2_clear_part==0);fm1_ms+=60;su2_ui_poll();assert(su2_overlay_was);snap("rec-cleared");fm1_ms+=1200;su2_ui_poll();assert(!su2_overlay_was);
 end_record=rec=ctl_seq_run=0;end_mode=0;pat_queue=ctl_pat_swap=ctl_snd_swap=0;
 /* Review defaults to CANCEL. Ordinary PLAY action never escapes the page. */
 init_page();su2_init_target=0;seq_trk=1;patch_t before=live_p[1],other=live_p[2];mixset_t mx;mx_from_set(&mx);pat.st[1][0].note[0]=61;pattern_t saved=pat;
 unsigned writes[4];memcpy(writes,host_write_count,sizeof writes);press(BTN_PLAY);assert(su2_init_stage==1&&!su2_init_choice);press(BTN_PLAY);assert(!su2_init_stage&&!memcmp(&before,&live_p[1],80));
 press(BTN_PLAY);su2_init_turn(1);press(BTN_PLAY);assert(su2_init_stage==3&&trk_edited[1]);assert(!memcmp(&other,&live_p[2],80)&&!memcmp(&saved,&pat,sizeof pat));mixset_t now;mx_from_set(&now);assert(!memcmp(&now,&mx,sizeof mx));assert(!memcmp(writes,host_write_count,sizeof writes));assert(!memcmp(live_p[1].v+NPARAM,"INIT",4));
 assert(ctl_part_reset==2u);engine_events();assert(!ctl_part_reset);
 snap("init-patch-done");press(BTN_HOME);assert(su2_init_stage==0&&page==PG_PERFORM);init_page();press(BTN_HOME);assert(page==PG_GLOBAL);init_page();
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

 /* Success timeout returns without SAVE or normal PATCH side effects. */
 page=PG_INITIALIZE;su2_init_stage=3;su2_init_done_t=fm1_ms;fm1_ms+=2199;su2_ui_poll();assert(page==PG_INITIALIZE);fm1_ms++;su2_ui_poll();assert(page==PG_PERFORM&&!su2_init_stage);assert(!memcmp(writes,host_write_count,sizeof writes));
 /* K5 owns synth tempo across every page and during save UI. */
 end_mode=rec=end_record=0;for(unsigned pg=0;pg<NPAGE;pg++){page=pg;layer=0;set.tempo=110;ctl_tempo=110;memset(host_enc,0,sizeof host_enc);host_enc[ENC_SELECT]=3;panel_encoders();assert(set.tempo==113&&ctl_tempo==113);}
 preset_ui=1;host_enc[ENC_SELECT]=2;panel_encoders();assert(set.tempo==115);preset_ui=0;
 /* Pattern ABI / metadata and both manual save paths. */
 assert(sizeof(pattern_t)==14476);pat_init(&pat);assert(!pattern_tempo(&pat));pattern_tempo_set(&pat,137);pattern_focus_set(&pat,2);assert(pattern_tempo(&pat)==137&&pattern_focus(&pat)==2);
 unsigned oldslot=set.pat;set.pat=0;set.tempo=143;ctl_tempo=143;pat_queue=pat_dirty=copy_to=pend_slot=0;pat_save_to(0);assert(pattern_tempo(&pat)==143);pat_dirty=pat_save_say=0;
 set.tempo=155;copy_arm=2;copy_arm_t=fm1_ms;pat_copy(1);assert(pattern_tempo(&pat)==155);copy_to=0;set.pat=oldslot;
 /* A missing legacy value never changes the current tempo. MASTER skips it. */
 pat_init(&pat);ctl_pattern_tempo=0;set.tempo=ctl_tempo=121;su3_source_set(1);assert(ctl_tempo==121);pattern_tempo_set(&pat,147);su3_source_set(0);assert(ctl_tempo==121);su3_source_set(1);assert(ctl_tempo==147&&set.tempo==147);
 mm_data.reserved[0]=0x5A;mm_data.reserved[1]=3;zm_boot();assert(zm_zoom&&ctl_pattern_tempo);mm_data.reserved[1]=1;zm_boot();assert(zm_zoom&&!ctl_pattern_tempo);
 /* The audio-boundary swap changes tempo before destination events. */
 for(unsigned mode=0;mode<2;mode++){pat_init(&pat);pat_init(&pat_q);pattern_tempo_set(&pat_q,180);ctl_pattern_tempo=mode;ctl_tempo=120;ctl_ext_quarter=0;ctl_seq_run=1;seq_was=0;ctl_count=0;ctl_pat_swap=1;ctl_pat_keep=0;end_mode=0;seq_tick();assert(!ctl_pat_swap&&ui_pat_swapped&&ctl_tempo==(mode?180:120));ctl_pat_swap=0;ctl_seq_run=0;seq_tick();}
 /* Zoom note/activity refresh never wipes the keyboard footer. */
 page=PG_KEYBOARD;layer=0;zm_zoom=1;seq_trk=0;draw_page();uint16_t footer[240*40];memcpy(footer,fb+240*200,sizeof footer);zm_activity();assert(!memcmp(footer,fb+240*200,sizeof footer));snap("keyboard-footer");
 for(unsigned i=0;i<2;i++){page=i?PG_SUPER:PG_SUB;draw_page();unsigned pixels=0;for(unsigned i=240*200;i<240*240;i++)pixels+=fb[i]!=C_PANEL;assert(pixels>20);}
 /* REC expiry forces a full Endless redraw, including unchanged controls. */
 page=PG_SEQ;end_mode=1;end_ui_seen=1;su2_overlay_was=1;su2_clear_t=rec_down=0;fm1_ms+=5000;su2_ui_poll();assert(!end_ui_seen&&!su2_overlay_was);endless_draw(!end_ui_seen);end_ui_seen=1;snap("endless-restored");
 page=PG_TEMPO;su3_tempo_draw();snap("tempo-source");

 /* Persist source without committing edited MIDI maps; CRC readback/boot. */
 end_record=rec=ctl_seq_run=0;end_mode=0;mm_on=0;notes_down=0;fm1_in.keys=0;engine_init();mm_boot();mm_default(&mm_data.profile[0]);mm_data.profile[0].map[4].num=77;assert(!mm_store_save(mm_slot,&mm_data,sizeof mm_data));mm_data.profile[0].map[4].num=88;
 ctl_pattern_tempo=0;su3_source_set(1);fm1_ms+=1600;zm_theme_poll();assert(!zm_theme_pending);mm_store_t persisted;assert(!mm_store_load(mm_slot,&persisted,sizeof persisted));assert(persisted.profile[0].map[4].num==77&&(persisted.reserved[1]&2));assert(mm_data.profile[0].map[4].num==88);mm_boot();zm_boot();assert(ctl_pattern_tempo);
 /* Actual pattern storage and stopped recall, including old prefix fallback. */
 pat_init(&pat);pattern_tempo_set(&pat,137);assert(!pattern_slot_save(0,&pat,&psnd[0]));ctl_pattern_tempo=0;set.tempo=ctl_tempo=119;load_pattern(0);assert(ctl_tempo==119);ctl_pattern_tempo=1;load_pattern(0);assert(ctl_tempo==137&&set.tempo==137);
 pat_init(&pat);assert(!pattern_slot_save(0,&pat,&psnd[0]));set.tempo=ctl_tempo=119;load_pattern(0);assert(ctl_tempo==119&&!pattern_tempo(&pat));
 /* MODO MIDI keeps the K5 CC mapping and never calls synth tempo handling. */
 mm_on=1;mm_settings=mm_edit=mm_reset_ask=mm_kbank=0;mm_values[4]=20;unsigned prevtempo=set.tempo;host_enc[4]=2;mm_input();assert(mm_values[4]>20&&set.tempo==prevtempo);mm_on=0;
 puts("PASS SU3: tempo metadata/save/copy; MASTER/PATTERN audio-boundary recall; legacy fallback; K5 on every synth page; init auto return; keyboard footer preservation; SUB/SUPER voices; Endless cache invalidation. PASS SU2: master real controls; six colored mixer pages; SINGLE actual-part identity; levels/octaves; Endless READY/play/REC; REC progress/abort/ack; init cancel/busy/target-change/execution; parts and manual flash preservation; INIT + basic DRUM-X, 16 empty steps per track.");
}
