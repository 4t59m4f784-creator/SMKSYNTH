/* SPDX-License-Identifier: GPL-3.0-only
 * SU3: transport tempo and display integration. Existing pattern/map ABI. */
#pragma once
static uint32_t su3_last_bpm;
static unsigned su3_bpm(void){return ctl_ext_quarter?60u*FS/ctl_ext_quarter:ctl_tempo;}
static void su3_bpm_text(char*s){fmt_u(s,su3_bpm());scat(s,ctl_ext_quarter?"BPM E":"BPM");}
static void su3_bpm_draw(int x,int y){char s[16];su3_bpm_text(s);zm_text(x-(int)zm_width(s,16),y,s,16,C_CREAM);}
static void su3_tempo_turn(int e){
 unsigned bpm=(unsigned)clampi((int)set.tempo+e,40,240);if(bpm==set.tempo)return;
 set.tempo=(uint8_t)bpm;ctl_tempo=bpm;set_dirty=1;set_t=fm1_ms;su2_redraw();
}
static void su3_source_set(unsigned source){
 source=!!source;if(source==ctl_pattern_tempo)return;ctl_pattern_tempo=source;
 if(source&&pattern_tempo(&pat)){set.tempo=(uint8_t)pattern_tempo(&pat);ctl_tempo=set.tempo;set_dirty=1;set_t=fm1_ms;}
 mm_data.reserved[0]=0x5A;mm_data.reserved[1]=(uint8_t)(zm_zoom|(source<<1));
 zm_theme_pending=1;zm_theme_t=fm1_ms;su2_redraw();
}
static void su3_tempo_draw(void){
 for(unsigned y=0;y<240;y+=120){mui_begin(y);zm_head("TEMPO SOURCE","GLOBE",'G');
  zm_center(120,62,"K1 SOURCE",16,C_AMBER);zm_center(120,85,ctl_pattern_tempo?"PATTERN":"MASTER",32,C_CREAM);
  char s[24];s[0]=0;scat(s,"K5 ");su3_bpm_text(s+slen(s));zm_center(120,135,s,32,C_CREAM);
  s[0]=0;scat(s,"PATTERN: ");unsigned bpm=pattern_tempo(&pat);if(bpm){fmt_u(s+slen(s),bpm);scat(s," BPM");}else scat(s,"NO TEMPO");zm_center(120,182,s,16,C_DIMT);
  zm_center(120,222,zm_theme_pending?"WAIT: STOP / RELEASE":"SAVE PATTERN TO STORE BPM",16,C_DIMT);mui_end(y);
 }
}
static void su3_ui_poll(void){
 uint32_t bpm=su3_bpm();if(bpm==su3_last_bpm)return;su3_last_bpm=bpm;
 if(mm_on)return;
 dirty_hdr=dirty_page=1;zm_footer_hash=0;
}
