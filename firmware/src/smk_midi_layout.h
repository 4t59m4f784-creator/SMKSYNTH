/* SPDX-License-Identifier: GPL-3.0-only */
/* MIDI large UI: full coordinates, rendered into the EXISTING 240x120 strip.
 * No second framebuffer and no changes to map storage or MIDI routing. */
#pragma once
#ifndef SMK_MIDI_UI_STAGE
#define SMK_MIDI_UI_STAGE 2
#endif
static int mui_eq(const char*a,const char*b){while(*a&&*a==*b){a++;b++;}return *a==*b;}
static void mui_copy(char*d,const char*s){while((*d++=*s++));}
static int mui_full_page(unsigned pg){return pg==PG_MIDI||pg==PG_CONTROLS;}
static void mui_begin(unsigned y){cv_begin(240,120,C_PANEL);cv_oy=-(int)y;}
static void mui_end(unsigned y){cv_oy=0;cv_blit(0,y);}
static void mui_center(int x,int y,const felucca_font_t *f,const char *s,uint16_t c){cv_text(x-text_w(f,s)/2,y,f,s,c);}
static void mui_border(int x,int y,int w,int h,uint16_t c){cv_rect(x,y,w,1,c);cv_rect(x,y+h-1,w,1,c);cv_rect(x,y,1,h,c);cv_rect(x+w-1,y,1,h,c);}
static void mui_header(const char *title,int map){
 char s[12];cv_text(8,0,&FONT_L,title,C_CREAM);cv_rect(8,32,224,2,C_RED);
 if(map){s[0]=0;scat(s,"MAP");fmt_u(s+3,mm_data.selected+1u);if(mm_dirty)scat(s,"*");cv_text(232-text_w(&FONT_S,s),9,&FONT_S,s,C_RED);}
}
static void mui_knob(int cx,int cy,int r,unsigned value,unsigned max,uint16_t color,int disabled){
 int a=225-(int)(270u*value/(max?max:1)),px,py,qx,qy;
 cv_arc(cx,cy,r+4,225,-45,C_RULE);if(!disabled)cv_arc(cx,cy,r+4,225,a,color);
 cv_disc(cx,cy,r,0,C_KNOB);cv_disc(cx,cy,r,1,disabled?C_RULE:C_KNOB_HI);
 if(!disabled){knob_xy(cx,cy,4,a,&px,&py);knob_xy(cx,cy,r-3,a,&qx,&qy);cv_line2(px,py,qx,qy,C_CREAM);}
}
static void mui_u2(char*s,unsigned v){if(v<10){s[0]='0';fmt_u(s+1,v);}else fmt_u(s,v);}
static void mui_u3(char*s,unsigned v){if(v<100){s[0]='0';mui_u2(s+1,v);}else fmt_u(s,v);}
static void mui_big(int cx,int y,const char *s,unsigned scale,uint16_t color){
 if(scale<2){mui_center(cx,y,&FONT_L,s,color);return;}
 const felucca_font_t *f=&FONT_L;int x=cx-(int)(text_w(f,s)*scale/2u);
 for(;*s;s++){
  unsigned gi=glyph(f,(uint8_t)*s),w=f->bw[gi],bpr=(w+1u)/2u;const uint8_t *p=f->data+f->off[gi];
  for(unsigned gy=0;gy<f->h;gy++)for(unsigned gx=0;gx<w;gx++){
   unsigned a=p[gy*bpr+gx/2u];a=gx&1u?a&15u:a>>4;
   if(a)for(unsigned sy=0;sy<scale;sy++)for(unsigned sx=0;sx<scale;sx++)cv_pset(x+((int)gx-(int)f->pad)*(int)scale+(int)sx,y+(int)(gy*scale+sy),blend565(0,color,a*256u/15u));
  }
  x+=(int)(f->adv[gi]*scale);
 }
}
static void mui_cell(unsigned i,int editing,const char *label,const char *value,unsigned v,unsigned max,uint16_t color,int off){
 char b[24];int x=8+(int)(i%2u)*116,y=editing?52+(int)(i/2u)*86:38+(int)(i/2u)*92,h=editing?82:88;
 cv_rect(x,y,108,h,RGB(19,25,36));mui_border(x,y,108,h,C_RULE);
 b[0]='K';b[1]=(char)('1'+i);b[2]=' ';b[3]=0;scat(b,label);cv_text(x+7,y+2,&FONT_S,b,off?C_DIMT:C_AMBER);
 mui_knob(x+54,y+(editing?31:37),editing?12:14,v,max,color,off);
 mui_center(x+54,y+(editing?45:54),&FONT_L,value,off?C_DIMT:color);
}
static const char *mui_footer(const char *normal){
 if(mui_eq(mm_status,"SAVED"))return "SAVED";
 if(mui_eq(mm_status,"SAVE FAILED / RETRY"))return "SAVE FAILED / RETRY";
 if(mui_eq(mm_status,"RELEASE KEYS / PEDAL"))return "RELEASE KEYS / PEDAL";
 if(mui_eq(mm_status,"MIDI PANIC")||mui_eq(mm_status,"PANIC / TX OVERFLOW"))return "MIDI PANIC";
 return normal;
}
static void mui_entry_draw(void){
 for(unsigned y=0;y<240;y+=120){
  mui_begin(y);mui_header("MIDI MODE",0);mui_center(120,48,&FONT_L,"K1 MODE",C_AMBER);
  mui_knob(120,121,30,0,1,C_AMBER,0);mui_big(120,170,"SYNTH",1,C_CREAM);
  cv_text(8,224,&FONT_S,"GLOBE BACK",C_DIMT);mui_end(y);
 }
}
static void mui_config_draw(void){
 char map[8],ch[8];mui_u2(map,mm_data.selected+1u);mui_u2(ch,mm_main_ch());
 for(unsigned y=0;y<240;y+=120){
  mui_begin(y);mui_header("MIDI SETUP",1);
  mui_cell(0,0,"MODE","MIDI",1,1,C_CREAM,0);mui_cell(1,0,"MAP",map,mm_data.selected,3,C_CREAM,0);
  mui_cell(2,0,"CHANNEL",ch,mm_main_ch()-1u,15,C_CREAM,0);mui_cell(3,0,"ENCODER",mm_relative()?"REL":"ABS",mm_relative(),1,C_CREAM,0);
  cv_text(8,224,&FONT_S,mui_footer("BT SAVE    GLOBE BACK"),C_DIMT);mui_end(y);
 }
}
static void mui_reset_draw(void){
 char b[8];fmt_u(b,mm_data.selected+1u);
 for(unsigned y=0;y<240;y+=120){
  mui_begin(y);mui_header("MIDI MODE",1);mui_center(120,46,&FONT_L,"RESET MAP",C_AMBER);mui_big(120,84,b,2,C_CREAM);
  cv_rect(8,165,108,56,RGB(19,25,36));mui_border(8,165,108,56,C_RED);mui_center(62,166,&FONT_S,"BT",C_DIMT);mui_center(62,185,&FONT_L,"RESET",C_RED);
  cv_rect(124,165,108,56,RGB(19,25,36));mui_border(124,165,108,56,C_AMBER);mui_center(178,166,&FONT_S,"PATCH",C_DIMT);mui_center(178,185,&FONT_L,"BACK",C_AMBER);mui_end(y);
 }
}

static void mui_edit_header(const char *name){
 char b[12];cv_text(8,0,&FONT_S,"MIDI EDIT",C_AMBER);b[0]=0;scat(b,"MAP");fmt_u(b+3,mm_data.selected+1u);if(mm_dirty)scat(b,"*");cv_text(232-text_w(&FONT_S,b),0,&FONT_S,b,C_RED);
 cv_text(8,14,&FONT_L,name,C_CREAM);cv_rect(8,48,224,2,C_RED);
}
static void mui_controls_draw(unsigned jump){
 for(unsigned y=0;y<240;y+=120){
  mui_begin(y);mui_header("CONTROLS",0);cv_text(192,9,&FONT_S,"SYNTH",C_AMBER);
  mui_center(120,47,&FONT_L,"K1 FADERS",C_AMBER);mui_knob(120,123,30,!jump,1,C_AMBER,0);
  mui_big(120,174,jump?"JUMP":"CATCH",1,C_CREAM);cv_text(8,224,&FONT_S,"GLOBE BACK",C_DIMT);mui_end(y);
 }
}
