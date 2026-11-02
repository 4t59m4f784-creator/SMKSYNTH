/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <stdio.h>
#include <assert.h>
int main(void){
 static uint16_t before[240*44];unsigned changed=0,cases=0;
 for(unsigned t=0;t<4;t++)for(unsigned prog=1;prog<=99;prog++){
  cv_begin(240,44,C_PANEL);cv_seg7(6,6,prog/10,C_TRKC[t],C_RED_DIM);cv_seg7(26,6,prog%10,C_TRKC[t],C_RED_DIM);cv_text(54,4,&FONT_S,"D SOLO LEAD *",C_TRKC[t]);memcpy(before,cv_px,sizeof before);
  int x=240-4-text_w(&FONT_S,"OBERVAVE");draw_logo_small(x,4);
  for(unsigned y=0;y<44;y++)for(int i=0;i<x;i++)assert(before[y*240+i]==cv_px[y*240+i]);
  for(unsigned i=0;i<240*44;i++)changed+=before[i]!=cv_px[i];cases++;
 }
 assert(changed>0);splash_pads(0,0,256);for(unsigned p=0;p<16;p++)assert(!padled_rgb[p]);
 splash_pads(256,0,256);unsigned r=(padled_rgb[0]>>8)&255,b=padled_rgb[0]&255;assert(r>b);
 splash_pads(256,120,256);assert((padled_rgb[0]&255)>((padled_rgb[0]>>8)&255));assert(((padled_rgb[7]>>8)&255)>(padled_rgb[7]&255));
 for(unsigned p=0;p<8;p++)assert(padled_rgb[p]==padled_rgb[p+8]);
 splash_pads(256,300,0);for(unsigned p=0;p<16;p++)assert(!padled_rgb[p]);
 printf("PASS: %u headers preserve all pixels outside logo; boot colours, left-to-right sweep, both rows and final blackout.\n",cases);return 0;
}
