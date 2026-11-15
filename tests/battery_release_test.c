/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static uint16_t before[57600];
static void reset_meter(unsigned raw){smk_battery_level=smk_battery_ms=smk_battery_average=smk_battery_pending=smk_battery_count=0;fm1_ms=0;fm1_in.batt=raw;}
static void sample(void){fm1_ms+=1000;smk_battery_poll();}
static unsigned pixels(int x,int y,int w,int h,uint16_t c){unsigned n=0;for(int yy=y;yy<y+h;yy++)for(int xx=x;xx<x+w;xx++)n+=fb[yy*240+xx]==c;return n;}
int main(void){
 const unsigned raw[]={520,530,560,602,800},levels[]={1,2,3,4,4};
 for(unsigned i=0;i<5;i++){reset_meter(raw[i]);sample();sample();assert(!smk_battery_level);sample();assert(smk_battery_level==levels[i]);}
 reset_meter(559);for(unsigned i=0;i<6;i++){fm1_in.batt=i&1?560:559;sample();}assert(smk_battery_level==2);
 reset_meter(800);for(unsigned i=0;i<3;i++)sample();fm1_in.batt=0;sample();assert(!smk_battery_level&&!smk_battery_average);fm1_in.batt=1023;sample();assert(!smk_battery_level);
 engine_init();pf_init();pat_init(&pat);mm_boot();page=PG_PERFORM;seq_trk=0;layer=0;
 for(unsigned theme=0;theme<2;theme++){
  zm_zoom=theme;smk_battery_level=0;lcd_fill(0,0,240,240,C_PANEL);draw_header();draw_page();memcpy(before,fb,sizeof before);
  smk_battery_level=4;draw_header();draw_page();
  /* The live meter must change no parameter, pattern, voice or part pixels. */
  int x=theme?181:213,y=theme?34:6;
  for(unsigned yy=0;yy<240;yy++)for(unsigned xx=0;xx<240;xx++)if(!(xx>=(unsigned)x&&xx<(unsigned)x+23&&yy>=(unsigned)y&&yy<(unsigned)y+12))assert(fb[yy*240+xx]==before[yy*240+xx]);
  assert(pixels(x,y,23,12,C_CREAM)>100);char path[80];snprintf(path,sizeof path,"build/ema/battery-theme-%u.ppm",theme);write_ppm_as(path);
 }
 smk_battery_level=1;draw_page();assert(pixels(181,34,23,12,C_RED)>40);
 page=PG_THEME;draw_page();assert(pixels(8,31,160,16,C_DIMT)>80);write_ppm_as("build/ema/global-version.ppm");
 /* Splash title is outside every animated logo frame. */
 lcd_fill(0,0,240,240,C_PANEL);cv_begin(240,38,C_PANEL);cv_text((240-text_w(&FONT_S,SMK_RELEASE_NAME))/2,0,&FONT_S,SMK_RELEASE_NAME,C_CREAM);cv_text((240-text_w(&FONT_S,APP_VER))/2,19,&FONT_S,APP_VER,C_DIMT);cv_blit(0,199);memcpy(before,fb,sizeof before);splash_frame(256,240,256);
 for(unsigned y=199;y<240;y++)for(unsigned x=0;x<240;x++)assert(fb[y*240+x]==before[y*240+x]);write_ppm_as("build/ema/splash-version.ppm");
 puts("PASS: stock four battery bands, three-second confirmation, filtered boundary noise, invalid ADC, two themes preserve all pixels outside meter; Global release label and splash footer survive animation.");
}
