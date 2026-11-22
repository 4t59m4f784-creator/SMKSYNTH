/* SPDX-License-Identifier: GPL-3.0-only
 * SU2: isolated UI additions. No new persisted fields or audio allocations. */
#pragma once
static uint32_t su2_clear_t,su2_clear_part,su2_overlay_was,su2_ui_t,su2_init_swallow,su2_init_seen,su2_init_fingerprint;
static void su2_redraw(void){dirty_hdr=dirty_tags=dirty_page=dirty_steps=1;hints_key=0xFFFFFFFFu;scope_state=1;zm_footer_hash=0;}
static unsigned su2_end_status(char*s){
 unsigned t=seq_trk%NTRK,len=pat.trk[t].len;s[0]=0;
 if(!end_mode||!endless_enabled(&pat.trk[t]))return 0;
 scat(s,"ENDLESS ");
 if(end_record==t+1u){scat(s,"REC");return 2;}
 if(!ctl_seq_run||!len){scat(s,"READY");return 1;}
 fmt_u(s+slen(s),ui_seq_pos[t]%len+1u);scat(s,"/");fmt_u(s+slen(s),len);return 1;
}
static const char*su2_pad_name(void){switch(pf_bank()){
 case 1:return "PADS: STEPS";case 2:return "PADS: PATTERNS";
 case 3:return "PADS: SPLIT/LAYER";case 4:return "PADS: DRUM-X";
 default:return "PADS: PARTS/FAV";}}
static void su2_dots(int x,int step,int y,int radius){
 for(unsigned i=0;i<NCARD;i++){
  int cx=x+(int)i*step;
  if(obv_drums_selected()){
   unsigned hit=drum_stamp[seq_trk%NTRK][i]&&eng_t+1u-drum_stamp[seq_trk%NTRK][i]<FS/8u;
   uint16_t c=hit?C_RED:C_KNOB_HI;int r=radius,half=r/2?r/2:1;
   /* Compact versions of the existing percussion silhouettes, never voice
    * meters: kicks/toms, snare, clap, hats, rim and bell follow drum stamps. */
   if(i==0||i==5){cv_disc(cx,y,r,1,c);cv_line(cx-half,y+r,cx-r,y+r+1,c);cv_line(cx+half,y+r,cx+r,y+r+1,c);}
   else if(i==1){mui_border(cx-r,y-half,2*r,2*half,c);cv_line(cx-r+1,y,cx+r-1,y,c);}
   else if(i==2){cv_line(cx-r,y-r,cx-half,y+r,c);cv_line(cx,y-r,cx+half,y+r,c);cv_line(cx+half,y-r,cx+r,y+r,c);}
   else if(i==3||i==4){cv_line(cx-r,y,cx+r,y,c);cv_line(cx,y-r,cx,y+r,c);if(i==4)cv_line(cx-r+1,y-half,cx+r-1,y-half,c);}
   else if(i==6){cv_disc(cx,y,r,1,c);cv_line(cx-r,y-r,cx+r,y+r,c);}
   else{cv_line(cx,y-r,cx-r,y+r,c);cv_line(cx-r,y+r,cx+r,y+r,c);cv_line(cx+r,y+r,cx,y-r,c);}
  }else{int32_t level=0;for(unsigned v=i;v<NV;v+=NCARD)if(ui_vlevel[v]>level)level=ui_vlevel[v];
   uint16_t c=blend565(C_RED_DIM,RGB(255,70,40),(unsigned)clampi(level>>7,0,255));cv_disc(cx,y,radius,0,c);}
 }
}
/* Called inside either a full-screen canvas or a y=200 footer strip. */
static int su2_footer_body(void){
 if(mm_on||layer||save_ui_active()||is_seq_page(page)||page==PG_INITIALIZE||page==PG_THEME||(page==PG_KEYBOARD&&zm_zoom))return 0;
 if(page!=PG_PERFORM&&page!=PG_MIXER&&page!=PG_KEYBOARD)return 0;
 if(!zm_zoom&&page==PG_MIXER&&!zm_scope_active())return 0;
 if(page==PG_KEYBOARD){cv_rect(0,200,240,40,C_PANEL);su2_dots(22,28,207,3);cv_rect(8,219,224,1,C_RULE);zm_text(8,223,su2_pad_name(),16,C_CREAM);char ctx[8]={'E',(char)('1'+kb_edit_row),'/','P',(char)('1'+pat.keyboard_selected),0};zm_text(232-(int)zm_width(ctx,16),223,ctx,16,C_DIMT);return 1;}
 char status[32];unsigned active=su2_end_status(status);
 cv_rect(0,200,240,40,C_PANEL);
 if(zm_scope_active()){
  if(active){char line[36]={(char)('A'+seq_trk%NTRK),' ',0};scat(line,status);zm_text(8,200,line,16,active==2?C_RED:C_AMBER);su2_dots(22,28,231,6);}
  else su2_dots(22,28,224,6);
  return 1;
 }
 if(active){zm_text(8,200,status,16,active==2?C_RED:C_AMBER);su2_dots(142,12,207,3);}
 else su2_dots(22,28,207,3);
 cv_rect(8,219,224,1,C_RULE);zm_text(8,223,su2_pad_name(),16,C_CREAM);return 1;
}
static void su2_mixer_colors(void){
 for(unsigned i=0;i<4;i++){int x=4+(i%2)*120,y=54+(i/2)*76;char l[2]={(char)('A'+i),0};
 cv_begin(112,2,C_TRKC[i]);cv_blit(x,y);cv_begin(30,27,RGB(19,25,36));cv_rect(1,1,24,25,C_TRKC[i]);zm_center(13,1,l,24,C_CREAM);cv_blit(x+27,y+4);}
}
static void su2_keyboard_draw(void){
 const keyboard_cfg_t*k=keyboard_row(&pat,kb_edit_row),*play=keyboard_live(&pat);unsigned combined=keyboard_active();
 for(unsigned y=0;y<240;y+=120){mui_begin(y);
  zm_text(8,2,KEYBOARD_MODE[k->mode],32,C_CREAM);zm_text(128,12,"K1 MODE",16,C_AMBER);ui_pattern_badge(208,4);smk_battery_icon(209,34);
  char info[32],v[12];info[0]=0;scat(info,"EDIT");fmt_u(info+slen(info),kb_edit_row+1u);scat(info," / PLAY");fmt_u(info+slen(info),pat.keyboard_selected+1u);scat(info," ");
  unsigned n=slen(info);info[n]=(char)('A'+(combined?play->part[0]:seq_trk%NTRK));info[n+1]=0;
  if(combined){n=slen(info);info[n]='+';info[n+1]=(char)('A'+play->part[1]);info[n+2]=0;}
  zm_text(8,33,info,16,C_DIMT);cv_rect(8,51,224,2,C_TRKC[seq_trk%NTRK]);
  for(unsigned i=0;i<2;i++){
   /* SINGLE plays the selected part, NOT a remembered pair. When outside that
    * pair, show the actual live timbre separately instead of claiming it plays. */
   unsigned stored=k->part[i],t=k->mode==KB_SINGLE&&!i?seq_trk%NTRK:stored;
   unsigned inactive=k->mode==KB_SINGLE&&i;uint16_t color=inactive?C_RULE:C_TRKC[t],ink=inactive?C_DIMT:C_CREAM;
   int yy=56+(int)i*54;char name[PATCH_NAME+1],letter[2]={(char)('A'+t),0};zm_name(name,&live_p[t]);
   mui_border(8,yy,224,50,C_RULE);cv_rect(9,yy+1,2,48,color);cv_rect(16,yy+8,24,32,color);zm_text(20,yy+8,letter,32,ink);zm_text(48,yy+2,name,24,ink);
   if(inactive)zm_text(202,yy+7,"OFF",16,C_DIMT);else if(k->mode==KB_SPLIT)zm_text(i?194:202,yy+7,i?"HIGH":"LOW",16,C_AMBER);
   mui_u3(v,trk_prog[t]+1u);zm_text(48,yy+28,v,16,inactive?C_DIMT:C_TRKC[t]);
   char kl[12]={'K',(char)('2'+i),' ',0};scat(kl,kb_edit_oct?"OCT":"LEVEL");
   /* In SINGLE, a stored level must never be mislabelled as the selected
    * part's gain when that selected part is not the stored first member. */
   if(k->mode==KB_SINGLE&&stored!=t){zm_text(92,yy+28,"COMBO OFF",16,C_DIMT);}
   else{zm_text(92,yy+28,kl,16,C_DIMT);v[0]=0;if(kb_edit_oct)fmt_s(v,k->octave[i]);else{fmt_u(v,k->level[i]);scat(v,"%");}zm_text(174,yy+22,v,24,k->mode==KB_SINGLE?C_DIMT:C_CREAM);}
  }
  zm_text(8,175,"K4 SPLIT",16,C_DIMT);fmt_note(v,k->split);zm_text(152,166,v,32,k->mode==KB_SPLIT?C_CREAM:C_DIMT);
  zm_text(8,202,"K5 LEVEL / OCT",16,C_AMBER);cv_rect(8,220,224,1,C_RULE);zm_text(8,223,su2_pad_name(),16,C_CREAM);mui_end(y);
 }
}
static void su2_cleared(void){su2_clear_t=fm1_ms|1u;su2_clear_part=seq_trk%NTRK;}
static int su2_init_busy(void){return pat_queue||ctl_pat_swap||ctl_snd_swap||pb_busy()||pat_dirty||copy_to||pat_save_say||manual_store_request||rec||end_record;}
static void su2_init_turn(int e){
 if(su2_init_stage>=2u)return;
 if(su2_init_stage){su2_init_choice=e>0;}
 else{su2_init_target=e>0;su2_init_part=seq_trk%NTRK;su2_init_pattern=set.pat;}
 su2_redraw();
}
static void su2_init_execute(void){
 unsigned t=su2_init_part;
 if(!su2_init_target){
  patch_t p;patch_from_factory(&p,&FACT_INIT);if(!t)fx_keep(&p);
  fm1_irq_off();live_p[t]=p;ctl_part_reset|=1u<<t;fm1_irq_on();
  memset(kb_refs[t],0,sizeof kb_refs[t]);for(unsigned n=0;n<128;n++){kb_press_t*q=&kb_press[n];unsigned w=0;for(unsigned i=0;i<q->count;i++)if(q->route[i].part!=t)q->route[w++]=q->route[i];q->count=(uint8_t)w;}
  trk_edited[t]=1;obv_park_valid[t]=0;pf_active[t]=255;smk_controls_epoch++;
 }else{
  /* Transport and voices were already stopped by the existing panic path.
   * Like stopped pattern recall, prepare the full pattern outside IRQ masks. */
  pat_init(&pat_q);pat=pat_q;end_mode=0;endless_reset();
  for(unsigned i=0;i<NTRK;i++){
   patch_t p;patch_from_factory(&p,&FACT_INIT);if(i==3u)obv_drums_preset(&p,0);
   p.v[P_ARP_MODE]=0;p.v[P_CHO_MIX]=p.v[P_DLY_MIX]=p.v[P_REV_MIX]=0;
   live_p[i]=p;trk_prog[i]=64;set.tprog[i]=64;trk_edited[i]=1;obv_park_valid[i]=0;pf_active[i]=255;
   set.mix_lvl[i]=100;set.mix_pan[i]=0;set.mix_cho[i]=set.mix_dly[i]=set.mix_rev[i]=set.mix_drv[i]=0;
  }
  set.mix_mute=set.mix_solo=0;set.m_vol=100;set.m_dj=64;set.m_drive=set.m_comp=0;
  set.dv_amt=set.dv_mix=0;set.prog=64;fx_sync();mix_apply();
  seq_trk=0;kb_edit_row=kb_edit_oct=0;keyboard_pattern_epoch++;keyboard_pattern_gains(&pat);
  memset(seq_cur,0,sizeof seq_cur);memset(seq_pg,0,sizeof seq_pg);smk_controls_epoch++;
 }
 pat_edited=1;pat_edit_t=fm1_ms|1u;set_dirty=1;set_t=fm1_ms;snd_capture(set.pat);
 /* No prog_put, pattern_slot_save, save request or favourite mutation. */
 su2_init_stage=3;su2_redraw();
}
static uint32_t su2_init_buttons(uint32_t b,uint32_t*rel){
 const uint32_t mask=(1u<<BTN_PLAY)|(1u<<BTN_HOME);
 uint32_t blocked=su2_init_swallow;su2_init_swallow&=~*rel;*rel&=~blocked;b&=~blocked;
 if(page!=PG_INITIALIZE||layer){su2_init_seen=0;su2_init_stage=0;return b;}
 if(!su2_init_seen){su2_init_seen=1;su2_init_stage=su2_init_choice=0;su2_init_part=seq_trk%NTRK;su2_init_pattern=set.pat;pf_home_down=0;}
 if(su2_init_stage==1u&&!su2_init_target&&su2_init_fingerprint!=zm_hash(&live_p[su2_init_part],sizeof(patch_t))){su2_init_stage=su2_init_choice=0;su2_redraw();}
 if(su2_init_stage&&((su2_init_part!=seq_trk%NTRK&&!su2_init_target)||su2_init_pattern!=set.pat)){su2_init_stage=su2_init_choice=0;su2_init_part=seq_trk%NTRK;su2_init_pattern=set.pat;su2_redraw();}
 uint32_t used=b&mask;su2_init_swallow|=used;
 if(b>>BTN_HOME&1u){if(su2_init_stage==2u){/* already authorized; finish first */}else if(su2_init_stage){su2_init_stage=su2_init_choice=0;su2_redraw();}else{page=PG_GLOBAL;su2_init_seen=0;su2_redraw();}}
 else if(b>>BTN_PLAY&1u){
  if(!su2_init_stage){su2_init_part=seq_trk%NTRK;su2_init_pattern=set.pat;su2_init_stage=1;su2_init_choice=0;su2_init_fingerprint=zm_hash(&live_p[su2_init_part],sizeof(patch_t));}
  else if(su2_init_stage==1u){
   if(!su2_init_choice)su2_init_stage=0;
   else if(su2_init_busy())say("FINISH RECORD / SAVE FIRST");
   else if(su2_init_target){smk_panel_panic();su2_init_stage=2;}
   else su2_init_execute();
  }else if(su2_init_stage==3u){su2_init_stage=0;su2_init_choice=0;}
  su2_redraw();
 }
 /* Other buttons cancel the confirmation before retaining their normal action. */
 if(b&~mask){su2_init_stage=su2_init_choice=0;su2_redraw();}
 b&=~mask;*rel&=~mask;return b;
}
static void su2_init_draw(void){
 char num[8],name[PATCH_NAME+1],line[40];unsigned t=su2_init_stage?su2_init_part:seq_trk%NTRK;
 for(unsigned y=0;y<240;y+=120){mui_begin(y);zm_head(su2_init_stage==3?"INITIALIZED":"INITIALIZE","",'G');
  if(su2_init_stage==2){zm_center(120,84,"STOPPING",24,C_CREAM);zm_center(120,132,"PLEASE WAIT",24,C_DIMT);}
  else if(su2_init_stage==3){zm_center(120,79,su2_init_target?"PATTERN RESET":"PATCH RESET",24,C_CREAM);zm_center(120,119,"NOT SAVED",24,C_AMBER);zm_center(120,160,"SAVE TO KEEP",16,C_DIMT);}
  else{
   zm_center(120,60,su2_init_target?"PATTERN":"PART PATCH",24,C_CREAM);
   if(su2_init_target){mui_u2(num,set.pat+1u);zm_center(120,91,num,40,C_AMBER);zm_center(120,139,"ALL 4 PARTS",24,C_CREAM);zm_center(120,170,"ALL SEQUENCES",16,C_RED);}
   else{line[0]=0;scat(line,"PART ");unsigned n=slen(line);line[n]=(char)('A'+t);line[n+1]=0;zm_center(120,91,line,32,C_TRKC[t]);zm_name(name,&live_p[t]);zm_center(120,132,name,24,C_CREAM);mui_u3(num,trk_prog[t]+1u);zm_center(120,161,num,16,C_TRKC[t]);}
   zm_center(120,188,su2_init_stage?(su2_init_choice?"K1 INITIALIZE":"K1 CANCEL"):"K1 TARGET",16,su2_init_stage&&su2_init_choice?C_RED:C_AMBER);
  }
  zm_center(120,205,su2_init_stage==2?"PLEASE WAIT":su2_init_stage==3?"PLAY: BACK":su2_init_stage?"PLAY: CONFIRM":"PLAY: CONTINUE",16,C_CREAM);zm_center(120,223,su2_init_stage==2?"":su2_init_stage==3?"PATCH: BACK":su2_init_stage?"PATCH: CANCEL":"PATCH: EXIT",16,C_DIMT);mui_end(y);
 }
}
static void su2_ui_poll(void){
 if(su2_init_stage==2u&&page==PG_INITIALIZE&&!ctl_panic&&engine_quiet()&&!notes_down&&!fm1_in.keys)su2_init_execute();
 if(fm1_ms-su2_ui_t<50u)return;su2_ui_t=fm1_ms;
 if(page==PG_INITIALIZE&&!layer)return;
 if(zm_zoom&&!layer&&!save_ui_active()&&(page==PG_PERFORM||page==PG_MIXER))zm_activity();
 if(!zm_zoom&&(page==PG_PERFORM||page==PG_KEYBOARD)&&!layer&&!save_ui_active()){cv_begin(240,40,C_PANEL);cv_oy=-200;su2_footer_body();cv_oy=0;cv_blit(0,200);}
 if(!zm_zoom&&page==PG_MIXER&&!layer&&!save_ui_active()){
  char s[32];unsigned en=su2_end_status(s);if(en){cv_begin(148,18,C_PANEL);cv_text(0,0,&FONT_S,s,en==2?C_RED:C_AMBER);cv_blit(54,22);}
 }
 /* Overlay reflects the EXISTING clear threshold and only acknowledges trk_clear. */
 unsigned held=rec_down?fm1_ms-rec_down_t:0,cleared=!rec_down&&su2_clear_t&&fm1_ms-su2_clear_t<1100u;
 unsigned overlay=zm_zoom&&!mm_on&&(held>=100u||cleared);
 if(overlay){
  unsigned t=cleared?su2_clear_part:seq_trk%NTRK;
  cv_begin(224,112,C_PANEL);mui_border(0,0,224,112,C_TRKC[t]);
  char l[16]="PART ";l[5]=(char)('A'+t);l[6]=0;zm_center(112,6,l,24,C_TRKC[t]);zm_center(112,39,cleared?"SEQ CLEARED":"HOLD REC",24,cleared?C_CREAM:C_RED);
  if(!cleared){zm_center(112,71,"TO CLEAR SEQUENCE",16,C_DIMT);cv_rect(12,96,200,5,C_RULE);cv_rect(12,96,(int)clampi(held,0,1200)*200/1200,5,C_RED);}
  cv_blit(8,65);
 }else if(su2_overlay_was){lcd_fill(0,0,240,240,C_PANEL);su2_redraw();if(is_seq_page(page))dirty_steps=1;else draw_page();}
 su2_overlay_was=overlay;
}
