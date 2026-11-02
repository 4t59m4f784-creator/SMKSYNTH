/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(void){
 engine_init();pf_init();pat_init(&pat);seq_trk=0;preset_part=0;fm1_ms=100;
 char name[40];name[0]=0;save_to=0;save_ui_target(name);assert(!strcmp(name,"FACTORY: DARK ARP"));
 save_to=98;name[0]=0;save_ui_target(name);assert(!strncmp(name,"FACTORY: INIT",13));
 patch_t p;prog_get(0,&p);memcpy(p.v+NPARAM,"MY BASS   ",PATCH_NAME);prog_put(42,&p);save_to=42;name[0]=0;save_ui_target(name);assert(!strcmp(name,"USER: MY BASS"));
 preset_ui=1;save_ui_dirty=1;unsigned fills=host_lcd_fills;save_ui_draw();assert(host_lcd_fills==fills);unsigned blits=host_lcd_blits,pixels=host_lcd_pixels;
 write_ppm_as("build/p16f1-save-user.ppm");
 for(unsigned i=0;i<1000;i++){fm1_ms++;save_ui_draw();}assert(host_lcd_blits==blits&&host_lcd_pixels==pixels);
 save_to=98;save_ui_dirty=1;save_ui_draw();assert(host_lcd_blits==blits+2&&host_lcd_fills==fills);write_ppm_as("build/p16f1-save-init.ppm");
 /* Browsing a target neither loads it nor overwrites the user patch. */
 prog_get(42,&p);assert(!memcmp(p.v+NPARAM,"MY BASS   ",PATCH_NAME));
 preset_ui=0;pb_confirm=16;pb_confirm_t=fm1_ms;save_ui_dirty=1;save_ui_draw();write_ppm_as("build/p16f1-overwrite-16.ppm");
 blits=host_lcd_blits;pixels=host_lcd_pixels;
 for(unsigned i=0;i<999;i++){fm1_ms++;save_ui_draw();}assert(host_lcd_blits==blits);
 fm1_ms++;save_ui_draw();assert(host_lcd_blits==blits+1&&host_lcd_pixels==pixels+240*20&&host_lcd_last_y==165&&host_lcd_last_h==20&&host_lcd_fills==fills);
 for(unsigned i=0;i<3000;i++){fm1_ms++;pf_hold_poll();if(save_ui_active())save_ui_draw();}assert(!pb_confirm&&host_lcd_blits==blits+3&&host_lcd_fills==fills);
 /* Waiting message changes once when transport stops, no periodic redraw. */
 preset_ui=2;ctl_seq_run=1;save_ui_dirty=1;save_ui_draw();blits=host_lcd_blits;
 fm1_ms+=100;save_ui_draw();assert(host_lcd_blits==blits);ctl_seq_run=0;save_ui_draw();assert(host_lcd_blits==blits+2);
 puts("PASS: factory/user/INIT target names without load/write; no clears, idle preset overlays zero LCD writes, countdown only 240x20 once/second, same4s cancel, one waiting-state repaint, native240x240 captures.");
}
