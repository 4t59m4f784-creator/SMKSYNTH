/* SPDX-License-Identifier: GPL-3.0-only
 * ZM2: an optional renderer and PERFORM view, not a second synth state.
 * Uses the existing 240x120 drawing strip. Theme uses two previously reserved
 * bytes in the existing dual-CRC map envelope; profile and bank ABI unchanged. */
#pragma once
static unsigned zm_zoom,zm_scope,zm_theme_pending,zm_theme_t;
static unsigned zm_focus_knob;
static void zm_mark_dirty(void);
static void zm_save_draw(unsigned mode,unsigned waiting,unsigned left);
static void zm_focus_capture(unsigned k,int p);
static uint32_t zm_focus_t,zm_focus_on,zm_footer_t,zm_footer_hash,zm_scope_t,zm_display_owner;
static char zm_focus_label[24],zm_focus_value[32];
static void zm_copy(char*d,const char*s,unsigned cap){unsigned i=0;while(i+1<cap&&s[i]){d[i]=s[i];i++;}d[i]=0;}
static void zm_boot(void){zm_zoom=mm_data.reserved[0]==0x5Au&&mm_data.reserved[1]==1u;zm_scope=zm_theme_pending=zm_focus_on=0;}
static void zm_theme_set(unsigned zoom){if(zm_zoom==!!zoom)return;zm_zoom=!!zoom;mm_data.reserved[0]=0x5A;mm_data.reserved[1]=(uint8_t)zm_zoom;zm_theme_pending=1;zm_theme_t=fm1_ms;zm_focus_on=0;}
static uint32_t zm_hash(const void*p,unsigned n){uint32_t h=2166136261u;const uint8_t*b=p;while(n--)h=(h^*b++)*16777619u;return h;}
/* Save ONLY the theme over the last committed maps. Never commit unsaved edits. */
static void zm_theme_poll(void){
 if(!zm_theme_pending||mm_on||ctl_seq_run||!engine_quiet()||fm1_ms-zm_theme_t<(zm_theme_pending==2u?5000u:1500u))return;
 if(mm_store_load(mm_slot,&mm_verify,sizeof mm_verify)||!mm_valid(&mm_verify)){
  memset(&mm_verify,0,sizeof mm_verify);mm_verify.tag=0x314D4D53u;mm_verify.version=1;
  for(unsigned i=0;i<4;i++)mm_default(&mm_verify.profile[i]);
 }
 unsigned seq=mm_data.seq+1u;if(!seq)seq=1;
 mm_verify.seq=seq;mm_verify.reserved[0]=0x5A;mm_verify.reserved[1]=(uint8_t)zm_zoom;
 uint32_t h=zm_hash(&mm_verify,sizeof mm_verify);unsigned slot=mm_slot^1u;
 int bad=mm_store_save(slot,&mm_verify,sizeof mm_verify)||mm_store_load(slot,&mm_verify,sizeof mm_verify)||!mm_valid(&mm_verify)||zm_hash(&mm_verify,sizeof mm_verify)!=h;
 if(!bad){mm_data.seq=seq;mm_slot=slot;zm_theme_pending=0;}else {zm_theme_pending=2;zm_theme_t=fm1_ms;}
 zm_mark_dirty();
}
static int zm_scope_active(void){return zm_scope&&page==PG_PERFORM&&!layer&&!mm_on;}
static int zm_transpose_active(void){return zm_zoom&&page==PG_SEQ&&seq_view==SV_TRANSPOSE&&!layer;}
static int zm_grid_active(void){return zm_zoom&&!is_seq_page(page)&&layer!=L_SEQ&&page!=PG_MIDI&&page!=PG_CONTROLS&&!mm_on;}
static int zm_full(void){return (page==PG_THEME&&!layer)||zm_scope_active()||zm_grid_active()||zm_transpose_active();}
static void zm_text(int x,int y,const char*s,unsigned size,uint16_t color){
 const felucca_font_t*f=size>=32?&FONT_L:&FONT_S;
 for(;*s;s++){
  unsigned gi=glyph(f,(uint8_t)*s),w=f->bw[gi],bpr=(w+1)/2;const uint8_t*p=f->data+f->off[gi];
  unsigned width=(w*size+f->h-1u)/f->h;
  for(unsigned yy=0;yy<size;yy++){
   int dy=y+(int)yy;if((unsigned)(dy+cv_oy)>=cv_h)continue;unsigned sy=yy*f->h/size;
   for(unsigned xx=0;xx<width;xx++){int dx=x+(int)xx-(int)(f->pad*size/f->h);if((unsigned)dx>=cv_w)continue;
    unsigned sx=xx*f->h/size,a=p[sy*bpr+sx/2u];a=sx&1?a&15:a>>4;
    if(a)cv_pset(dx,dy,a==15?color:blend565(cv_get(dx,dy),color,a*256u/15u));
   }
  }
  x+=(int)(f->adv[gi]*size/f->h);
 }
}
static unsigned zm_width(const char*s,unsigned size){return text_w(size>=32?&FONT_L:&FONT_S,s)*size/(size>=32?32u:16u);}
static void zm_center(int x,int y,const char*s,unsigned size,uint16_t c){zm_text(x-(int)zm_width(s,size)/2,y,s,size,c);}
static void zm_name(char*s,const patch_t*p){unsigned n=PATCH_NAME;while(n&&p->v[NPARAM+n-1]==' ')n--;for(unsigned i=0;i<n;i++)s[i]=(char)p->v[NPARAM+i];s[n]=0;}
static void zm_value(int x,int y,const char*s,unsigned width,uint16_t c){
 unsigned n=0;while(s[n]&&((s[n]>='0'&&s[n]<='9')||s[n]=='+'||s[n]=='-'||s[n]=='.'))n++;
 if(n){char num[16];unsigned k=n<15?n:15;memcpy(num,s,k);num[k]=0;const char*u=s+n;unsigned size=n<=4?40:32,total=zm_width(num,size)+(*u?4+zm_width(u,16):0);if(total>width){size=32;total=zm_width(num,size)+(*u?4+zm_width(u,16):0);}int left=x+((int)width-(int)total)/2;zm_text(left,y,num,size,c);if(*u)zm_text(left+(int)zm_width(num,size)+4,y+(int)size-18,u,16,C_AMBER);}
 else {unsigned size=slen(s)<=6?32:24;while(size>16&&zm_width(s,size)>width)size-=4;zm_center(x+(int)width/2,y+(40-(int)size)/2,s,size,c);}
}
static uint16_t zm_part_color(void){return C_TRKC[seq_trk%NTRK];}
static void zm_cell(unsigned i,const knob_view_t*k){
 int x=4+(i%2)*120,y=54+(i/2)*76;
 cv_rect(x,y,112,74,RGB(19,25,36));mui_border(x,y,112,74,C_RULE);if(!k->present)return;
 char key[3]={'K',(char)('1'+i),0};cv_rect(x+4,y+5,20,20,C_PANEL);mui_border(x+4,y+5,20,20,C_AMBER);zm_text(x+6,y+6,key,16,C_CREAM);
 const char*label=k->label;unsigned size=24;while(size>16&&zm_width(label,size)>85)size-=2;
 zm_text(x+27,y+4,label,size,C_CREAM);zm_value(x+4,y+31,k->value,104,C_CREAM);
}
static void zm_head(const char*title,const char*sub,unsigned part){
 unsigned size=24;while(size>16&&zm_width(title,size)>195)size-=2;
 uint16_t c=part>='A'&&part<='D'?C_TRKC[part-'A']:C_AMBER;
 zm_text(8,3,title,size,C_CREAM);char l[2]={(char)part,0};cv_rect(210,4,22,26,c);zm_center(221,5,l,24,C_CREAM);
 if(part>='A'&&part<='D'&&slen(sub)>=4u&&sub[0]>='0'&&sub[0]<='9'&&sub[1]>='0'&&sub[1]<='9'&&sub[2]>='0'&&sub[2]<='9'&&sub[3]==' '){
  char prog[4];memcpy(prog,sub,3);prog[3]=0;zm_text(8,31,prog,16,c);zm_text(8+(int)zm_width(prog,16),31,sub+3,16,C_DIMT);
 }else zm_text(8,31,sub,16,C_DIMT);
 cv_rect(8,50,224,2,c);
}
static void zm_focus_named(const char*l,const char*v){if(!(zm_zoom||zm_scope_active())||is_seq_page(page)||layer)return;zm_focus_knob=0;zm_copy(zm_focus_label,l,sizeof zm_focus_label);zm_copy(zm_focus_value,v,sizeof zm_focus_value);zm_focus_t=fm1_ms;zm_focus_on=1;}
static int zm_focus_visible(void){return zm_focus_on&&fm1_ms-zm_focus_t<1000u;}
static void zm_focus_draw(void){
 cv_rect(17,72,206,115,C_PANEL);mui_border(17,72,206,115,C_AMBER);
 unsigned size=32;while(size>16&&zm_width(zm_focus_label,size)>190)size-=2;zm_center(120,82,zm_focus_label,size,C_AMBER);
 zm_value(27,125,zm_focus_value,186,C_CREAM);
}
static void zm_activity_body(void){
 uint16_t accent=C_RED;
 if(zm_transpose_active()){zm_text(8,222,"DRUM-X FIXED",16,C_AMBER);return;}
 if(page==PG_KEYBOARD&&!zm_scope_active()){zm_text(8,222,"K5 LEVEL / OCT",16,C_AMBER);return;}
 if(!zm_scope_active()&&!(page<=PG_OUTPUT||page==PG_DKIT||page==PG_DSOUND))return;
 if(obv_drums_selected()){
  static const char*const names[]={"BD","SD","CP","CH","OH","TM","RM","BL"};
  for(unsigned i=0;i<8;i++){int x=22+(int)i*28;unsigned hit=drum_stamp[seq_trk%NTRK][i]&&eng_t+1u-drum_stamp[seq_trk%NTRK][i]<FS/8u;uint16_t c=hit?accent:C_KNOB_HI;if(hit)cv_rect(x-12,207,24,33,RGB(58,22,26));zm_text(x-8,207,names[i],16,hit?accent:C_AMBER);
   if(i==0||i==5){cv_disc(x,230,5,1,c);cv_line(x-3,235,x-4,238,c);cv_line(x+3,235,x+4,238,c);}else if(i==1){mui_border(x-6,226,12,8,c);cv_line(x-4,229,x+4,229,c);}else if(i==2){for(int k=-4;k<=4;k+=4)cv_line2(x+k-1,226,x+k+1,234,c);}else if(i==3||i==4){cv_line2(x-7,229,x+7,229,c);cv_line(x,225,x,236,c);if(i==4)cv_line(x-5,226,x+5,226,c);}else if(i==6){cv_disc(x,230,5,1,c);cv_line(x-5,225,x+5,235,c);}else{cv_line(x,224,x-6,234,c);cv_line(x-6,234,x+6,234,c);cv_line(x+6,234,x,224,c);cv_line(x-6,236,x+6,236,c);}
  }
 }else{
  for(unsigned i=0;i<NCARD;i++){int32_t level=0;for(unsigned v=i;v<NV;v+=NCARD)if(ui_vlevel[v]>level)level=ui_vlevel[v];unsigned bright=clampi(level>>7,0,255);int x=22+(int)i*28;uint16_t c=blend565(blend565(C_PANEL,accent,45),accent,bright);if(bright>8)cv_disc(x,224,9,0,blend565(C_PANEL,c,64));cv_disc(x,224,6,0,c);cv_disc(x,224,6,1,blend565(C_PANEL,c,170));}
 }
}
static void zm_activity(void){cv_begin(240,33,C_PANEL);cv_oy=-207;zm_activity_body();cv_oy=0;cv_blit(0,207);}
static void zm_scope_plot(void){
 for(unsigned y=60;y<198;y+=69){cv_begin(240,69,C_PANEL);cv_oy=-(int)y;cv_rect(8,60,224,138,C_SCOPE_BG);cv_line(8,129,231,129,C_SCOPE_MID);for(int x=64;x<232;x+=56)cv_line(x,60,x,197,C_SCOPE_MID);
  for(unsigned i=1;i<224;i++){int a=clampi(scope[(i-1u)*SCOPE_N/224u]>>8,-65,65),b=clampi(scope[i*SCOPE_N/224u]>>8,-65,65);cv_line((int)i+7,129-a,(int)i+8,129-b,C_AMBER);}
  if(zm_focus_visible())zm_focus_draw();cv_oy=0;cv_blit(0,y);
 }
}
static void zm_grid(const char*title,const knob_view_t*k){
 if(zm_focus_knob&&zm_focus_visible()){unsigned i=zm_focus_knob-1;if(i<4&&k[i].present){zm_focus_label[0]='K';zm_focus_label[1]=(char)('1'+i);zm_focus_label[2]=' ';zm_copy(zm_focus_label+3,k[i].label,sizeof zm_focus_label-3);zm_copy(zm_focus_value,k[i].value,sizeof zm_focus_value);}}
 char name[PATCH_NAME+1],sub[40],num[12];zm_name(name,ed_patch(0));sub[0]=0;mui_u3(num,ed_prog()+1u);scat(sub,num);scat(sub," ");scat(sub,name);
 const char*head=title;
 if(page==PG_KEYBOARD){sub[0]=0;scat(sub,kb_edit_row?"EDIT 2 / PLAY ":"EDIT 1 / PLAY ");fmt_u(sub+slen(sub),pat.keyboard_selected+1u);}
 if(obv_drums_selected()&&(page==PG_DKIT||page==PG_DSOUND)){head=DRUM_NAME[drum_ui_lane[seq_trk%NTRK]%8u];zm_copy(sub,page==PG_DKIT?"DRUM KIT":"DRUM SOUND",sizeof sub);}
 for(unsigned y=0;y<240;y+=120){mui_begin(y);
  if(zm_scope_active()){
   zm_center(120,8,"SMKSYNTH",32,C_CREAM);cv_stripes(26,2,2,48,192,1);cv_rect(8,49,224,2,C_RED);
  }else{
   zm_head(page==PG_PERFORM&&!layer?name:head,page==PG_PERFORM&&!layer?"":sub,'A'+seq_trk%NTRK);
   ui_pattern_badge(208,31);
   if(page==PG_PERFORM&&!layer){zm_text(8,29,num,24,zm_part_color());zm_text(60,33,"PERFORM",16,C_DIMT);}
   if(layer==L_GLO){for(unsigned i=0;i<4;i++){char l[2]={(char)('A'+i),0};zm_text(8,58+(int)i*36,l,32,C_AMBER);zm_text(48,61+(int)i*36,set.mix_mute>>i&1u?"MUTE":trk_silent(i)?"STOP":"PLAY",24,C_CREAM);}}
   else if(layer==L_SAVE){for(unsigned i=0;i<16;i++){char n[4];mui_u2(n,i+1u);int x=8+(int)(i%4)*57,yy=57+(int)(i/4)*35;cv_rect(x,yy,51,30,i==set.pat?C_RED:C_RULE);zm_center(x+26,yy+1,n,24,C_CREAM);}}
   else for(unsigned i=0;i<4;i++){zm_cell(i,&k[i]);app_idle();}
   zm_activity_body();if(zm_focus_visible())zm_focus_draw();
  }
  mui_end(y);
 }
 if(zm_scope_active()){zm_scope_plot();zm_activity();}
}
static void zm_theme_draw(void){for(unsigned y=0;y<240;y+=120){mui_begin(y);zm_head("THEME","GLOBAL / DISPLAY",'G');zm_center(120,65,"K1 THEME",24,C_AMBER);mui_knob(120,126,23,zm_zoom,1,C_AMBER,0);zm_center(120,171,zm_zoom?"ZOOM":"OBERVAVE",32,C_CREAM);zm_text(8,222,zm_theme_pending==2?"SAVE FAILED / RETRY":zm_theme_pending?"WAIT: STOP / RELEASE":"GLOBE NEXT",16,C_DIMT);mui_end(y);}}
