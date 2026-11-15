/* SPDX-License-Identifier: GPL-3.0-only */
#pragma once
/* Reused SMKTRON meter: SMK-37 stock ADC bands 530/560/602, as documented
 * by genkigenki/fm1-to-smk37 docs/01-hardware.md, section 6.6.
 * Approximate bands, not percent/charging detection. Existing mux reading;
 * no extra ADC work, flash writes, audio-ISR work or framebuffer. */
static unsigned smk_battery_level,smk_battery_ms,smk_battery_average;
static unsigned smk_battery_pending,smk_battery_count;
static int smk_battery_poll(void){
 unsigned raw=fm1_in.batt,old=smk_battery_level;
 if(fm1_ms-smk_battery_ms<1000u)return 0;
 smk_battery_ms=fm1_ms;
 if(!raw||raw>=1023u){smk_battery_count=smk_battery_average=smk_battery_pending=smk_battery_level=0;}
 else{
  smk_battery_average=smk_battery_average?(smk_battery_average*7u+(raw<<3))/8u:raw<<3;
  unsigned v=smk_battery_average>>3,next=1u+(v>=530u)+(v>=560u)+(v>=602u);
  if(next!=smk_battery_pending){smk_battery_pending=next;smk_battery_count=0;}
  if(next!=old&&++smk_battery_count>=3u)smk_battery_level=next;
 }
 return old!=smk_battery_level;
}
/* Exactly 23 x 12 pixels. Empty dim outline = ADC unavailable. */
static void smk_battery_icon(int x,int y){
 uint16_t c=smk_battery_level==1?C_RED:smk_battery_level?C_CREAM:C_DIMT;
 cv_rect(x,y,23,12,C_PANEL);
 cv_rect(x,y,20,1,c);cv_rect(x,y+11,20,1,c);
 cv_rect(x,y,1,12,c);cv_rect(x+19,y,1,12,c);cv_rect(x+20,y+4,3,4,c);
 for(unsigned i=0;i<smk_battery_level;i++)cv_rect(x+2+(int)i*4,y+3,3,6,c);
}
