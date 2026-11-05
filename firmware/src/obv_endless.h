/* SPDX-License-Identifier: GPL-3.0-only
 * ENDLESS E1: OP-1-inspired append workflow, original SMKSYNTH UI and player.
 * Same 256-step records/clock/voice allocation; experimental, SINGLE ANALOG.
 */
static int endless_screen(void){return page==PG_SEQ&&seq_view==SV_ENDLESS&&!layer;}
static void endless_finish(void){end_record=0;end_record_mask=0;end_count=0;}
static void endless_reset(void){
 for(unsigned n=0;n<128;n++)if(end_owner[n]&128u)keyboard_play_note(0,n,0);
 memset(end_owner,0,sizeof end_owner);endless_finish();
 for(unsigned t=0;t<NTRK;t++){end_keys[t]=0;end_transpose[t]=0;end_restart[t]++;}
 end_ui_seen=0;
}
static int endless_enable(void){
 unsigned t=seq_trk%NTRK;seqtrk_t*k=&pat.trk[t];
 if(obv_patch_engine(&live_p[t])!=OBV_ANALOG||keyboard_active()){say("ENDLESS: SINGLE ANALOG ONLY");return 0;}
 if(!endless_enabled(k)){
  for(unsigned n=0;n<128;n++)if(kb_press[n].count){say("RELEASE KEYS FIRST");return 0;}
  undo_save();unsigned used=0;
  for(unsigned n=0;n<k->len&&n<SEQ_STEPS;n++)used|=pat.st[t][n].note[0];
  for(unsigned l=0;l<NLANE;l++)used|=pat.lane[t][l].param!=AUTO_NONE;
  if(!used)k->len=0;
  k->pad[0]=0xD1u;k->pad[1]=96;pat_edited=1;end_ui_seen=0;
 }
 return 1;
}
static void endless_toggle(void){
 seqtrk_t*k=&pat.trk[seq_trk%NTRK];
 if(endless_enabled(k)){endless_reset();k->pad[0]=k->pad[1]=0;pat_edited=1;say("ENDLESS OFF / NOTES KEPT");}
 else if(endless_enable())say("ENDLESS ON / REC APPENDS");
}
static void endless_record_toggle(void){
 if(end_record){endless_finish();say("ENDLESS INPUT DONE");return;}
 if(ctl_seq_run||rec||pat_queue){say("STOP TRANSPORT FIRST");return;}
 if(!endless_enable())return;
 endless_reset();end_record=seq_trk+1u;end_record_mask=1u<<seq_trk;end_pos=pat.trk[seq_trk].len;
 say("REC INPUT: NOTES / RIGHT REST");
}
static int endless_note(uint32_t on,unsigned raw,unsigned vel){
 unsigned owner=end_owner[raw],t=seq_trk%NTRK;
 if(!on){
  if(!owner)return 0;
  end_owner[raw]=0;t=(owner&7u)-1u;
  if(owner&128u){keyboard_play_note(0,raw,0);if(end_record==t+1u&&end_count&&! --end_count)end_pos=pat.trk[t].len;}
  else if(end_keys[t])end_keys[t]--;
  return 1;
 }
 if(owner)endless_note(0,raw,0);
 if(layer||rec||step_held>=0||keyboard_active()||obv_patch_engine(&live_p[t])!=OBV_ANALOG||!endless_enabled(&pat.trk[t]))return 0;
 if(panel_live_note[raw]){
  unsigned had=panel_live_note[raw]==1u;keyboard_play_note(0,raw,0);panel_live_note[raw]=0;
  for(unsigned p=0;p<NTRK;p++)ctl_live_hold[p][raw>>5]&=~(1u<<(raw&31u));
  keyboard_record(0,raw,0);entry_note(0,raw,0);if(had&&notes_down)notes_down--;
 }
 if(end_record==t+1u){
  unsigned pitch;
  if(!end_count&&pat.trk[t].len>=SEQ_STEPS){say("ENDLESS FULL 256");return 1;}
  keyboard_play_note(1,raw,vel);
  if(!keyboard_input_pitch(raw,&pitch)){keyboard_play_note(0,raw,0);return 1;}
  if(!end_count){undo_save();end_pos=pat.trk[t].len;step_clear(seq_step(t,end_pos));pat.trk[t].len++;seq_step(t,end_pos)->gate=pat.trk[t].pad[1];seq_step(t,end_pos)->vel=(uint8_t)vel;}
  step_add(seq_step(t,end_pos),keyboard_stored_pitch(t,pitch),vel);
  end_owner[raw]=(uint8_t)(128u+t+1u);end_count++;pat_edited=1;return 1;
 }
 if(end_record)return 1;
 if(!pat.trk[t].len)return 0;
 int pitch=(int)raw+(keyboard_physical?12*panel_octave():0);
 if(pitch<0||pitch>127)return 1;
 end_owner[raw]=(uint8_t)(t+1u);
 if(!end_keys[t])end_restart[t]++;
 end_transpose[t]=pitch-48;end_keys[t]++;
 if(!ctl_seq_run){ctl_seq_restart=1;ctl_seq_run=1;}
 return 1;
}
static void endless_arrow(unsigned right){
 unsigned t=seq_trk%NTRK;seqtrk_t*k=&pat.trk[t];
 if(end_record==t+1u){
  if(!right){if(end_count){say("RELEASE KEYS BEFORE UNDO");return;}if(k->len){undo_save();step_clear(seq_step(t,--k->len));if(k->len&&seq_step(t,k->len-1u)->gate>=GATE_TIE)seq_step(t,k->len-1u)->gate=k->pad[1];end_pos=k->len;pat_edited=1;}return;}
  if(k->len>=SEQ_STEPS){say("ENDLESS FULL 256");return;}
  undo_save();
  if(end_count){seq_step(t,end_pos)->gate=GATE_TIE;*seq_step(t,k->len)=*seq_step(t,end_pos);seq_step(t,k->len)->gate=k->pad[1];end_pos=k->len++;}
  else {step_clear(seq_step(t,k->len));k->len++;end_pos=k->len;}
  pat_edited=1;
 }else if(k->len){seq_cursor_set(t,(seq_cur[t]+k->len+(right?1u:k->len-1u))%k->len);}
}
static void endless_turn(unsigned knob,int e){
 if(!endless_enable())return;
 seqtrk_t*k=&pat.trk[seq_trk%NTRK];int sign=e>0?1:-1;
 if(!knob)k->div=(uint8_t)clampi((int)k->div+sign,0,NDIV-1);
 else if(knob==1)k->pad[1]=(uint8_t)clampi((int)k->pad[1]+e,1,126);
 else if(knob==2){unsigned d=(unsigned)clampi((int)endless_dir(k)+sign,0,2);k->pad[0]=(uint8_t)((k->pad[0]&~6u)|(d<<1));end_restart[seq_trk]++;}
 else k->pad[0]=(uint8_t)((k->pad[0]&~1u)|(e>0));
 pat_edited=1;
}
static void endless_knob(knob_view_t*k,unsigned i){
 static const char*const labels[]={"RATE","GATE","DIR","HOLD"};
 const seqtrk_t*t=&pat.trk[seq_trk%NTRK];unsigned en=endless_enabled(t);
 k->label=labels[i];k->present=1;k->bip=0;k->value[0]=0;
 if(!i){scat(k->value,DIV_NAME[t->div%NDIV]);k->v=t->div;k->max=NDIV-1;}
 else if(i==1){unsigned gate=en?t->pad[1]:96;fmt_u(k->value,(gate+1u)*100u/128u);scat(k->value,"%");k->v=gate;k->max=126;}
 else if(i==2){unsigned d=en?endless_dir(t):0;scat(k->value,d==0?"FWD":d==1?"REV":"RND");k->v=d;k->max=2;}
 else{unsigned hold=en?endless_hold(t):1;scat(k->value,hold?"ON":"OFF");k->v=hold;k->max=1;}
}
static void endless_poll(void){
 static uint16_t epoch;
 if(epoch!=keyboard_pattern_epoch){epoch=keyboard_pattern_epoch;endless_reset();}
 if(end_record&&(!endless_screen()||seq_trk+1u!=end_record||ctl_seq_run||layer||keyboard_active()||obv_drums_selected()))endless_finish();
 if(ctl_panic)endless_reset();
 for(unsigned t=0;t<NTRK;t++)if(end_keys[t]&&(keyboard_active()||obv_patch_engine(&live_p[t])!=OBV_ANALOG)){endless_reset();break;}
}
/* Big digits are separate 56x88 strips; a tick never repaints the full LCD. */
static void endless_digit(int x,int y,unsigned digit,uint16_t color){
 static const unsigned seg[]={0x3f,6,0x5b,0x4f,0x66,0x6d,0x7d,7,0x7f,0x6f};unsigned m=seg[digit%10];
 if(m&1)cv_rect(x+6,y,36,10,color);
 if(m&2)cv_rect(x+38,y+6,10,34,color);
 if(m&4)cv_rect(x+38,y+46,10,34,color);
 if(m&8)cv_rect(x+6,y+74,36,10,color);
 if(m&16)cv_rect(x,y+46,10,34,color);
 if(m&32)cv_rect(x,y+6,10,34,color);
 if(m&64)cv_rect(x+6,y+37,36,10,color);
}
static void endless_draw(unsigned force){
 static unsigned header,body,foot,digits[3],oldcolor;
 unsigned t=seq_trk%NTRK,len=pat.trk[t].len,en=endless_enabled(&pat.trk[t]);
 unsigned running=ctl_seq_run&&en&&(endless_hold(&pat.trk[t])||end_keys[t])&&len;
 unsigned pos=end_record==t+1u?end_pos:running?ui_seq_pos[t]%len:len?seq_cur[t]%len:0u;
 unsigned num=pos+1u;if(num>256u)num=256u;
 uint16_t color=end_record?C_RED:en?RGB(255,166,40):C_DIMT;
 unsigned hk=t|(en<<3)|(running<<4)|(end_record<<5)|(trk_prog[t]<<10);
 unsigned toasting=toast&&fm1_ms-toast_t<1500u;
 hk^=toasting?toast_t+1u:0u;
 for(unsigned n=0;n<PATCH_NAME;n++)hk=hk*33u+live_p[t].v[NPARAM+n];
 if(force||hk!=header){
  header=hk;cv_begin(240,44,C_PANEL);char title[24]="ENDLESS ";title[8]=TRK_L[t];title[9]=0;cv_text(8,2,&FONT_S,title,color);
  const char*state=end_record?"REC":!en?"OFF":running?"PLAY":"STOP";cv_text(232-text_w(&FONT_S,state),2,&FONT_S,state,color);
  char name[29];unsigned nn=0;if(toasting){while(nn<28u&&toast[nn]){name[nn]=toast[nn];nn++;}}else{memcpy(name,live_p[t].v+NPARAM,PATCH_NAME);nn=PATCH_NAME;}name[nn]=0;cv_text(8,23,&FONT_S,name,C_DIMT);cv_blit(0,0);
 }
 unsigned ds[3]={num/100u,(num/10u)%10u,num%10u};
 if(force){lcd_fill(0,44,240,100,C_PANEL);}
 for(unsigned i=0;i<3;i++)if(force||digits[i]!=ds[i]||oldcolor!=color){digits[i]=ds[i];cv_begin(56,88,C_PANEL);endless_digit(4,2,ds[i],color);cv_blit(20+i*72u,48);}
 oldcolor=color;
 char note[32];note[0]=0;
 if(!en)scat(note,"PARA ENABLE / REC INPUT");
 else if(end_record&&!end_count)scat(note,"PLAY NOTE / > REST");
 else if(!len||pos>=len)scat(note,"EMPTY");
 else {const step_t*s=seq_step(t,pos);for(unsigned n=0;n<SEQ_CHORD&&s->note[n];n++){char nm[8];if(n)scat(note," ");fmt_note(nm,s->note[n]);scat(note,nm);}if(!s->note[0])scat(note,"REST");else if(s->gate>=GATE_TIE)scat(note," ~");}
 unsigned bh=pos*257u+len+color*65537u;for(unsigned n=0;note[n];n++)bh=bh*33u+(uint8_t)note[n];
 unsigned w0=pos/16u*16u;
 for(unsigned n=0;n<16&&w0+n<len;n++)bh=bh*17u+pat.st[t][w0+n].note[0];
 if(force||bh!=body){body=bh;cv_begin(240,48,C_PANEL);char total[16]="OF ";fmt_u(total+3,len);cv_text(8,0,&FONT_S,total,C_DIMT);
  if(en){char tr[12]="TR ";fmt_s(tr+3,end_transpose[t]);cv_text(232-text_w(&FONT_S,tr),0,&FONT_S,tr,C_DIMT);}
  cv_text((240-text_w(&FONT_S,note))/2,17,&FONT_S,note,color);
  for(unsigned n=0;n<16;n++){unsigned at=w0+n;if(at>=len)continue;cv_rect(8+(int)n*14,38,10,pat.st[t][at].note[0]?6:2,at==pos?C_CREAM:C_DIMT);}
  cv_blit(0,140);
 }
 unsigned fk=pat.trk[t].div|(pat.trk[t].pad[0]<<8)|(pat.trk[t].pad[1]<<16);
 if(force||fk!=foot){foot=fk;knob_view_t k[4];for(unsigned i=0;i<4;i++)endless_knob(&k[i],i);ui_seq_knobs(k);}
}
