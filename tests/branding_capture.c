/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(unsigned n){while(n--){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}}
static unsigned gates(unsigned p,unsigned seq){unsigned n=0;for(unsigned i=0;i<NV;i++)n+=vc[i].gate&&vc[i].part==p&&vc[i].seq_owned==seq;return n;}
static void off(void){for(unsigned n=0;n<128;n++)note_in(0,n,0);ctl_sustain=0;ctl_seq_run=0;ctl_panic=1;render(2);memset(sr,0,sizeof sr);}
static void init_parts(void){for(unsigned t=0;t<4;t++){trk_load(t,0);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_GLIDE]=0;live_p[t].v[P_ARP_MODE]=A_OFF;live_p[t].v[P_VINTAGE]=0;live_p[t].v[P_LFO_AMP]=0;set.mix_lvl[t]=100;}}
static void combo(unsigned mode){keyboard_cfg_t k;keyboard_defaults(&k);k.mode=mode;keyboard_changed(k);}
static void pad(unsigned p,unsigned vel){uint32_t d=0;host_qput(0,(int32_t)(p<<8|vel));smk_pads(48,&d);}
int main(void){engine_init();for(unsigned i=0;i<NPSECT;i++)psect[i].tag=PS_TAG;init_parts();set.m_vol=100;set.tempo=120;fx_sync();mix_apply();pf_init();pat_init(&pat);step_held=-1;layer=L_NONE;seq_trk=0;
 char f[100];for(unsigned p=0;p<NPAGE;p++){page=p;lcd_fill(0,0,240,240,C_PANEL);draw_header();draw_page();draw_scope();draw_leds();snprintf(f,sizeof f,"build/brand-screens/page-%02u.ppm",p);write_ppm_as(f);}
 for(unsigned mode=0;mode<3;mode++){kb_edit_row=1;combo(mode);page=PG_KEYBOARD;draw_header();draw_page();draw_keyboard_scope();snprintf(f,sizeof f,"build/brand-screens/keyboard-%u.ppm",mode);write_ppm_as(f);}
 for(unsigned k=0;k<30;k++){lcd_fill(0,0,240,240,C_PANEL);splash_frame(k<8?k*32:256,k<10?0:(k-10)*12,k<14?0:256);snprintf(f,sizeof f,"build/brand-screens/boot-%02u.ppm",k);write_ppm_as(f);}

 for(unsigned view=0;view<NSV;view++){page=PG_SEQ;seq_view=view;lcd_fill(0,0,240,240,C_PANEL);draw_seq_top();if(view==SV_TRANSPOSE)draw_seq_transpose();else if(view==SV_SLOTS)draw_seq_slots();else{draw_seq_roll();draw_seq_info();draw_seq_knobs();}snprintf(f,sizeof f,"build/brand-screens/sequence-%u.ppm",view);write_ppm_as(f);}
 obv_drums_choose(0,0);render(3);page=PG_DSEQ;seq_view=SV_STEPS;lcd_fill(0,0,240,240,C_PANEL);draw_seq_top();ui_drum_grid(0,0,0,0,0,0);draw_seq_knobs();write_ppm_as("build/brand-screens/drums-grid.ppm");
 lcd_fill(0,0,240,240,C_BLACK);draw_text_box(0,64,240,&FONT_S,"SMKSYNTH USB RESCUE",C_WHITE,1);draw_text_box(0,96,240,&FONT_S,"CONNECT USB",C_WHITE,1);draw_text_box(0,124,240,&FONT_S,"UNKNOWN FLASH",C_WHITE,1);draw_text_box(0,164,240,&FONT_S,"AUDIO OFF",C_WHITE,1);write_ppm_as("build/brand-screens/rescue.ppm");
 lcd_fill(0,0,240,240,C_RED);draw_text_box(0,20,240,&FONT_S,"SMKSYNTH CRASH",C_WHITE,1);write_ppm_as("build/brand-screens/crash-heading.ppm");
 assert(text_w(&FONT_S,"SMKSYNTH")<=text_w(&FONT_S,"OBERVAVE"));return 0;}
