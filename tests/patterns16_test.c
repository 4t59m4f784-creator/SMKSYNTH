/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static pattern_t full[16], back;
static psnd_t sounds[16], scene;
static int32_t audio[HALF_FRAMES*2];
static void render(unsigned n){while(n--){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}}
int main(void){
 engine_init();assert(NPAT==16&&sizeof(pattern_t)==14476&&sizeof(psnd_t)==380);
 for(unsigned i=0;i<16;i++){
  pattern_t *p=&full[i];pat_init(p);snd_capture(i);sounds[i]=psnd[i];
  pattern_focus_set(p,i%4);p->keyboard.mode=KB_LAYER;p->keyboard_alt.mode=KB_SPLIT;p->keyboard_alt.split=48+i;p->keyboard_selected=i%2;
  for(unsigned t=0;t<4;t++){
   p->trk[t].len=256;p->trk[t].swing=50+i;
   for(unsigned n=0;n<256;n++){
    for(unsigned c=0;c<4;c++)p->st[t][n].note[c]=36+(i+t+n+c)%60;
    p->st[t][n].vel=20+(i+n)%108;p->st[t][n].ratch=(n+i)%4;p->st[t][n].chance=(n+i)%101;p->st[t][n].nudge=(n+i)%20;
    drum_pattern_set(p,t,n,(n*17+i*13+t)&255);
    for(unsigned l=0;l<4;l++){p->lane[t][l].param=P_CUTOFF; p->lane[t][l].v[n]=(n+i+l)%128;}
   }
  }
  assert(!pattern_slot_save(i,p,&sounds[i]));
 }
 for(unsigned i=0;i<16;i++){assert(!pattern_slot_load(i,&back,&scene));assert(!memcmp(&full[i],&back,sizeof back));assert(!memcmp(&sounds[i],&scene,sizeof scene));}
 full[15].st[0][255].note[3]=111;assert(!pattern_slot_save(15,&full[15],&sounds[15]));
 for(unsigned i=0;i<16;i++){assert(!pattern_slot_load(i,&back,&scene));assert(!memcmp(&full[i],&back,sizeof back));}
 for(unsigned i=8;i<16;i++)assert(blob_load(i,&back,32));
 assert(pattern_slot_save(16,&full[0],&sounds[0]));
 pf_init();pf_manual_seq=2;pat_used=65535;set.pat=0;pat=full[0];seq_trk=2;fm1_ms=100;
 pb_event(15,100);fm1_ms+=800;pb_save(15);pb_event(15,0);assert(pb_confirm==16&&!copy_to);fm1_ms+=4000;pf_hold_poll();assert(!pb_confirm&&!copy_to);
 pb_event(15,100);fm1_ms+=800;pb_save(15);pb_event(15,0);assert(pb_confirm==16);pb_event(15,100);fm1_ms+=100;pb_event(15,0);assert(!pb_confirm&&copy_to==16&&pattern_focus(&pat)==2);
 copy_to=pb_write_target=pat_save_say=pat_dirty=0;
 pat_init(&pat);seq_trk=0;ctl_seq_transpose=2;ctl_seq_run=ctl_seq_restart=1;render(1);pat_go(15);render(500);assert(!ctl_pat_swap&&ui_pat_swapped&&pat_queue==16&&seq_trk==3&&ctl_seq_transpose==2);assert(pat.keyboard_selected==1&&pat.keyboard_alt.mode==KB_SPLIT);ctl_seq_run=0;ctl_panic=1;render(2);set.pat=15;pat_queue=ui_pat_swapped=0;
 toast=0;page=PG_SEQ;seq_view=SV_SLOTS;
 for(unsigned i=0;i<16;i++)for(unsigned t=0;t<4;t++)slot_len[i][t]=256;
 for(unsigned pg=0;pg<2;pg++){slot_sel=pg*8; lcd_fill(0,0,240,240,C_PANEL);draw_seq_top();draw_seq_slots();draw_seq_knobs();write_ppm_as(pg?"build/p16-page2.ppm":"build/p16-page1.ppm");}
 puts("PASS: 16 complete independent slots, 256 steps x4, chords x4, automation x4, 8 drum lanes, scenes, two keyboard rows + focus, overwrite slot16 isolated, pad16 hold/tap + timeout, legacy slots8..15 blocked, synchronized slot16 focus/keyboard recall with transpose retained, two native pages.");
}
