/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(unsigned n){while(n--){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}}
static unsigned active(unsigned p){unsigned n=0;for(unsigned v=0;v<NV;v++)n+=vc[v].gate&&vc[v].part==p&&vc[v].seq_owned;return n;}
static void key(unsigned on,unsigned note){panel_note_in(on,note,on?100:0);render(1);}
static void reset(void){
 endless_reset();ctl_seq_run=0;ctl_panic=1;render(2);pat_init(&pat);seq_trk=0;page=PG_SEQ;seq_view=SV_ENDLESS;layer=L_NONE;step_held=-1;rec=0;rec_down=0;pat_queue=0;transpose_keys=0;ctl_seq_transpose=0;
 memset(seq_cur,0,sizeof seq_cur);memset(kb_press,0,sizeof kb_press);memset(kb_refs,0,sizeof kb_refs);
 for(unsigned t=0;t<NTRK;t++){patch_from_factory(&live_p[t],&FACT_INIT);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_ARP_MODE]=0;live_p[t].v[P_VINTAGE]=0;}
 keyboard_pattern_gains(&pat);ctl_trk_on=15;ctl_trk_req=trk_req_seen=0;ctl_count=0;
}
int main(void){
 _Static_assert(sizeof(seqtrk_t)==8,"track record changed");_Static_assert(sizeof(pattern_t)==14476,"pattern changed");
 engine_init();pf_init();set.tempo=120;set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();
 for(unsigned t=0;t<4;t++){
  reset();seq_trk=t;assert(endless_enable());assert(!pat.trk[t].len);assert(endless_enabled(&pat.trk[t]));assert(!endless_enabled(&pat.trk[(t+1)%4]));
  endless_record_toggle();assert(end_record==t+1);
  key(1,60);key(1,64);key(1,67);assert(pat.trk[t].len==1&&end_count==3);key(0,60);key(0,64);assert(end_pos==0);key(0,67);assert(end_pos==1);
  endless_arrow(1);assert(pat.trk[t].len==2&&!seq_step(t,1)->note[0]);
  key(1,72);endless_arrow(1);assert(pat.trk[t].len==4&&seq_step(t,2)->gate==GATE_TIE&&seq_step(t,3)->note[0]==72);key(0,72);
  endless_arrow(0);assert(pat.trk[t].len==3&&seq_step(t,2)->gate<GATE_TIE);endless_finish();assert(!end_record_mask);
  /* All three directions remain bounded and gates/transpose belong to this part. */
  key(1,50);assert(ctl_seq_run&&end_transpose[t]==2);render(100);assert(ui_seq_pos[t]<3);key(0,50);
  pat.trk[t].pad[0]&=~1u;render(2);assert(!active(t));
  key(1,48);render(2);assert(active(t));key(0,48);render(2);assert(!active(t));
  ctl_seq_run=0;render(2);pat.trk[t].pad[0]=0xD3;ctl_seq_restart=ctl_seq_run=1;render(1);assert(ui_seq_pos[t]==2);
  pat.trk[t].pad[0]=0xD5;end_restart[t]++;render(400);assert(ui_seq_pos[t]<3);
  ctl_seq_run=0;render(2);endless_toggle();assert(!endless_enabled(&pat.trk[t])&&pat.trk[t].len==3);
 }
 reset();pat.st[0][0].note[0]=60;assert(endless_enable());assert(pat.trk[0].len==16&&pat.st[0][0].note[0]==60); /* never erase an existing phrase */
 reset();endless_record_toggle();for(unsigned i=0;i<256;i++)endless_arrow(1);assert(pat.trk[0].len==256);key(1,60);key(0,60);assert(pat.trk[0].len==256);endless_finish();
 /* A release goes to its captured owner after focus changes; no hung voice/gate. */
 reset();endless_record_toggle();key(1,60);seq_trk=1;endless_poll();key(0,60);assert(!kb_press[60].count&&!end_count);
 reset();endless_enable();pat.trk[0].len=1;pat.st[0][0].note[0]=60;pat.trk[0].pad[0]=0xD0;key(1,48);seq_trk=1;key(0,48);assert(!end_keys[0]);
 /* Focus change + repeated raw note must release old ENDLESS owner before live fallback. */
 reset();endless_enable();pat.trk[0].len=1;pat.st[0][0].note[0]=60;key(1,48);seq_trk=1;key(1,48);assert(!end_keys[0]&&kb_press[48].count);key(0,48);assert(!kb_press[48].count);
 /* Metadata survives actual slot save/load without growing records. */
 reset();endless_record_toggle();key(1,60);key(0,60);endless_finish();endless_turn(2,1);endless_turn(3,-1);pattern_focus_set(&pat,0);mx_from_set(&psnd[0].mx);psnd[0].magic=PSN2_MAGIC;
 assert(!pattern_slot_save(15,&pat,&psnd[0]));pattern_t loaded;psnd_t snd;assert(!pattern_slot_load(15,&loaded,&snd));assert(loaded.trk[0].pad[0]==pat.trk[0].pad[0]&&loaded.trk[0].len==1);
 /* Dirty rectangles: no idle redraw, only changed digit and lower step strip. */
 page=PG_SEQ;seq_view=SV_ENDLESS;toast=0;endless_draw(1);unsigned pix=host_lcd_pixels,blits=host_lcd_blits;for(unsigned i=0;i<1000;i++)endless_draw(0);assert(host_lcd_pixels==pix&&host_lcd_blits==blits);
 pat.trk[0].len=256;seq_cur[0]=127;endless_draw(0);write_ppm_as("build/endless-128.ppm");end_record=1;end_count=0;end_pos=7;endless_draw(0);write_ppm_as("build/endless-rec.ppm");
 endless_finish();seq_cur[0]=129;endless_draw(0);pix=host_lcd_pixels;seq_cur[0]=130;endless_draw(0);assert(host_lcd_pixels-pix<=56*88+240*48);
 reset();endless_enable();pat.trk[0].len=16;seq_cur[0]=6;pat.st[0][6].note[0]=60;pat.st[0][6].note[1]=64;pat.st[0][6].note[2]=67;ctl_seq_run=1;ui_seq_pos[0]=6;toast=0;endless_draw(1);write_ppm_as("build/endless-play.ppm");
 puts("PASS: 4 parts, chords/rests/ties/undo/256 bound, FWD/REV/RND, HOLD/transposition, focus-safe releases, legacy data preservation, flash roundtrip, idle/partial display.");
}
