/* SPDX-License-Identifier: GPL-3.0-only */
/* Capture the actual firmware splash, at its fully illuminated frame. */
#define main smksynth_host_main
#include "../firmware/src/obervave.c"
#undef main
int main(void){
 lcd_fill(0,0,240,240,C_PANEL);splash_frame(256,240,256);
 cv_begin(240,38,C_PANEL);
 cv_text((240-text_w(&FONT_S,SMK_RELEASE_NAME))/2,0,&FONT_S,SMK_RELEASE_NAME,C_CREAM);
 cv_text((240-text_w(&FONT_S,APP_VER))/2,19,&FONT_S,APP_VER,C_DIMT);
 cv_blit(0,199);
 FILE *f=fopen("build/release130/splash.ppm","wb");if(!f)return 1;
 fprintf(f,"P6\n240 240\n255\n");
 for(unsigned i=0;i<240u*240u;i++){
  uint16_t v=fb[i];unsigned char rgb[3]={(v>>11)*255u/31u,((v>>5)&63u)*255u/63u,(v&31u)*255u/31u};fwrite(rgb,1,3,f);
 }
 return fclose(f)!=0;
}
