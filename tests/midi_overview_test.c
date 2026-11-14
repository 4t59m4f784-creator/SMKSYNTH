/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void step(void){fm1_ms++;mm_input();mm_wheels_poll();mm_service();}
static void draw(void){fm1_ms+=100;mm_draw();}
static void key(unsigned k,unsigned down){if(down)fm1_in.keys|=(uint64_t)1<<k;else fm1_in.keys&=~((uint64_t)1<<k);fm1_in.key_vel[k]=100;step();}
static void pad(unsigned p,unsigned v){fm1_in.pad_st[p]=v?128:0;host_qput(0,p<<8|v);step();}
int main(void){engine_init();pf_init();pat_init(&pat);mm_boot();fm1_ms=1000;fm1_in.frames=400;mm_enter();for(unsigned i=0;i<120;i++)step();assert(mm_ui_control==MM_KEYS);
 for(unsigned i=0;i<8;i++)mm_cv(i,(int)(20+i*13),0);for(unsigned i=0;i<4;i++)mm_cv(16+i,(int)(30+i*18),0);mm_cv(2,105,0);assert(mm_ui_control==2&&mm_ui_control_v==105);
 pad(8,113);key(12,1);key(16,1);key(19,1);assert(mm_ui_control==2&&mm_ui_control_v==105&&mm_ui_pad==32&&mm_ui_pad_v==113);draw();assert(fb[230*240+81]==C_RED&&fb[230*240+101]==C_RED&&fb[230*240+121]==C_RED);write_ppm_as("build/mo1/overview-chord.ppm");
 unsigned pixels=host_lcd_pixels;key(12,0);key(16,0);key(19,0);assert(mm_screen_dirty);draw();assert(host_lcd_pixels-pixels==240u*57u);pixels=host_lcd_pixels;pad(8,0);assert(mm_ui_pad_v==113&&mm_ui_control_v==105);draw();assert(host_lcd_pixels-pixels==240u*63u);assert(fb[230*240+81]==C_CREAM);write_ppm_as("build/mo1/overview-released.ppm");
 /* A CC release after a key/control event must update only its own pad area. */
 mm_profile()->map[29]=(mm_entry_t){MM_CC,71,1};pad(5,100);mm_cv(3,67,0);key(0,1);pad(5,0);assert(mm_ui_pad==29&&!mm_ui_pad_v&&mm_ui_control==3&&mm_ui_control_v==67);key(0,0);
 mm_profile()->map[31]=(mm_entry_t){MM_TOGGLE,72,1};pad(7,100);pad(7,0);assert(mm_toggle[31]);draw();pad(7,100);pad(7,0);assert(!mm_toggle[31]);
 /* Original outgoing pitches remain the displayed names after OCT changes. */
 key(12,1);assert(mm_ui_note(12)==60);mm_oct=1;key(14,1);assert(mm_ui_note(12)==60&&mm_ui_note(14)==74);key(12,0);key(14,0);
 for(unsigned k=0;k<37;k++)key(k,1);draw();write_ppm_as("build/mo1/overview-37-keys.ppm");for(unsigned k=0;k<37;k++)key(k,0);draw();
 mm_cv(16,90,0);mm_fbank=1;mm_cv(20,38,0);assert(mm_ui_bar[16]==90&&mm_ui_bar[20]==38&&(mm_ui_known>>16&1u)&&(mm_ui_known>>20&1u));draw();write_ppm_as("build/mo1/overview-fader.ppm");
 mm_safety();assert(mm_ui_control==MM_KEYS&&mm_ui_pad==MM_KEYS&&!mm_ui_known);draw();assert(cv_w*cv_h<=240*120&&cv_oy==0);
 puts("PASS: independent controller/pad/key domains; 37-key chords and note-off repaint; pad CC release after other input; Toggle state; bank values; original pitches after octave change; safe reset and existing strip size.");
}
