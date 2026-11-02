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
int main(void){
 engine_init();for(unsigned i=0;i<NPSECT;i++)psect[i].tag=PS_TAG;init_parts();set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();pf_init();pat_init(&pat);step_held=-1;layer=L_NONE;seq_trk=0;pf_manual_seq=3;page=PG_KEYBOARD;
 pad(0,100);pad(1,100);pad(0,0);pad(1,0);assert(pat.keyboard.mode==KB_LAYER);
 note_in(1,60,100);render(2);assert(gates(0,0)&&gates(1,0));
 pad(10,100);pad(11,100);pad(10,0);pad(11,0);assert(kb_edit_row==1&&pat.keyboard_alt.mode==KB_LAYER&&pat.keyboard_alt.part[0]==2&&pat.keyboard_alt.part[1]==3);assert(pat.keyboard_selected==0);
 pad(13,100);pad(13,0);keyboard_turn(3,-1);keyboard_turn(1,-60);assert(pat.keyboard_alt.mode==KB_SPLIT&&pat.keyboard_alt.split==59&&pat.keyboard_alt.level[0]==40);
 assert(keyboard_pad_led(7)==RGB(255,255,255)&&keyboard_pad_led(15)!=RGB(255,255,255));
 pad(15,100);pad(15,0);assert(pat.keyboard_selected==1&&keyboard_pad_led(15)==RGB(255,255,255)&&keyboard_pad_led(7)!=RGB(255,255,255));
 note_in(1,58,100);note_in(1,59,100);render(2);assert(gates(2,0)&&gates(3,0)&&gates(0,0)&&gates(1,0));
 note_in(0,60,0);render(1);assert(!gates(0,0)&&!gates(1,0));note_in(0,58,0);note_in(0,59,0);render(1);assert(!gates(2,0)&&!gates(3,0));
 /* Independent gains even when configurations share a part. */
 keyboard_cfg_t k=pat.keyboard_alt;k.mode=KB_LAYER;k.part[0]=0;k.part[1]=2;k.level[0]=0;keyboard_changed(k);
 pad(7,100);pad(7,0);note_in(1,61,100);render(2);pad(15,100);pad(15,0);note_in(1,65,100);render(2);
 unsigned old=0,new=0;for(unsigned i=0;i<NV;i++)if(vc[i].gate&&vc[i].part==0){if(vc[i].key_combined==1){assert(vc[i].amp1>0);old++;}if(vc[i].key_combined==2){assert(vc[i].amp1==0);new++;}}assert(old&&new);off();
 /* Sustain across row changes, REC only selected member. */
 ctl_sustain=1;note_in(1,60,100);render(1);note_in(0,60,0);pad(7,100);pad(7,0);render(1);assert(gates(0,0)&&gates(2,0));ctl_sustain=0;render(1);assert(!gates(0,0)&&!gates(2,0));
 pad(15,100);pad(15,0);seq_trk=2;rec=REC_READY;fm1_ms=100;note_in(1,64,100);render(1);assert(pat.st[2][0].note[0]==64&&!pat.st[0][0].note[0]);fm1_ms+=200;note_in(0,64,0);rec=REC_OFF;off();
 snd_capture(0);psnd_t scene=psnd[0],back;pattern_t loaded,original=pat;assert(!pattern_slot_save(3,&pat,&scene));assert(!pattern_slot_load(3,&loaded,&back));assert(!memcmp(&original,&loaded,sizeof loaded)&&!memcmp(&scene,&back,sizeof scene));
 /* SL1 migration retains first row exactly, second row SINGLE and row1 active. */
 assert(!slot_save(4,&pat,PAT_SL1_BYTES,&scene,sizeof scene));assert(!pattern_slot_load(4,&loaded,&back));assert(!memcmp(&pat,&loaded,PAT_SL1_BYTES)&&loaded.keyboard_selected==0&&loaded.keyboard_alt.mode==KB_SINGLE);
 /* Synchronized pattern switch recovers both rows + selection; transpose global. */
 pat_init(&pat);ctl_seq_transpose=2;ctl_seq_run=ctl_seq_restart=1;render(1);pat_go(3);render(500);assert(!ctl_pat_swap&&pat.keyboard_selected==1&&pat.keyboard_alt.level[0]==0&&ctl_seq_transpose==2);off();
 /* Invalid selection and extension rejected. */
 loaded=original;loaded.keyboard_selected=2;assert(pattern_slot_save(5,&loaded,&scene));
 /* Native display for visual verification. */
 kb_edit_row=1;page=PG_KEYBOARD;draw_header();draw_page();draw_keyboard_scope();draw_hints();write_ppm_as("build/sl2-row2.ppm");
 puts("PASS: two independent rows, max white selector, inactive edit, split boundary, held releases, row-exclusive gains, sustain, REC, SL1 migration, two-row persistence, synchronized pattern swap and transpose.");
}
