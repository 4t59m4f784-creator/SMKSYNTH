/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(unsigned n){while(n--){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}}
static unsigned active(unsigned p){unsigned n=0;for(unsigned v=0;v<NV;v++)n+=vc[v].gate&&vc[v].part==p&&vc[v].seq_owned;return n;}
static void key(unsigned on,unsigned note){panel_note_in(on,note,on?100:0);render(1);for(unsigned i=0;on&&end_pending&&i<1000;i++)render(1);assert(!on||!end_pending);}
static void reset(void){
 endless_reset();end_mode=0;end_global_hold=1;end_stop_request=0;ctl_seq_run=0;ctl_panic=1;render(2);pat_init(&pat);seq_trk=0;page=PG_SEQ;seq_view=SV_ENDLESS;layer=L_NONE;step_held=-1;rec=0;rec_down=0;pat_queue=0;transpose_keys=0;ctl_seq_transpose=0;keyboard_physical=0;
 memset(seq_cur,0,sizeof seq_cur);memset(kb_press,0,sizeof kb_press);memset(kb_refs,0,sizeof kb_refs);
 for(unsigned t=0;t<NTRK;t++){patch_from_factory(&live_p[t],&FACT_INIT);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_ARP_MODE]=0;live_p[t].v[P_VINTAGE]=0;}
 keyboard_pattern_gains(&pat);ctl_trk_on=15;ctl_trk_req=trk_req_seen=0;ctl_count=0;
}
static void phrase(unsigned t,unsigned len){pat.trk[t].len=(uint16_t)len;for(unsigned n=0;n<len;n++){step_clear(seq_step(t,n));seq_step(t,n)->note[0]=(uint8_t)(48+t*12+n);}}
int main(void){
 _Static_assert(sizeof(seqtrk_t)==8,"track record changed");_Static_assert(sizeof(pattern_t)==14476,"pattern changed");
 engine_init();pf_init();set.tempo=120;set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();
 for(unsigned t=0;t<4;t++){
  reset();seq_trk=t;assert(endless_enable());assert(end_mode&&pat.trk[t].len==16);
  for(unsigned p=0;p<4;p++)assert(endless_enabled(&pat.trk[p]));
  endless_record_toggle();assert(end_record==t+1&&!pat.trk[t].len);
  key(1,60);key(1,64);key(1,67);assert(pat.trk[t].len==1&&end_count==3);key(0,60);key(0,64);assert(end_pos==0);key(0,67);assert(end_pos==1);
  endless_arrow(1);assert(pat.trk[t].len==2&&!seq_step(t,1)->note[0]);
  key(1,72);endless_arrow(1);assert(pat.trk[t].len==4&&seq_step(t,2)->gate==GATE_TIE&&seq_step(t,3)->note[0]==72);key(0,72);
  endless_arrow(0);assert(pat.trk[t].len==3&&seq_step(t,2)->gate<GATE_TIE);endless_finish();assert(!end_record_mask);
  endless_turn(3,-1);for(unsigned p=0;p<4;p++)assert(!endless_hold(&pat.trk[p]));
  key(1,50);assert(ctl_seq_run&&end_global_transpose==2);render(100);assert(ui_seq_pos[t]<3);key(0,50);assert(!ctl_seq_run&&!active(t));
  key(1,48);render(2);assert(active(t));key(0,48);assert(!ctl_seq_run&&!active(t));
  endless_turn(3,1);endless_turn(2,1);ctl_seq_restart=ctl_seq_run=1;render(1);assert(ui_seq_pos[t]==2);
  endless_turn(2,1);render(400);assert(ui_seq_pos[t]<3);
  endless_toggle();assert(!end_mode&&!ctl_seq_run&&pat.trk[t].len==3);for(unsigned p=0;p<4;p++)assert(!endless_enabled(&pat.trk[p]));
 }
 /* Preserve data and intentional recorded rests, including first entry into another part. */
 reset();pat.st[0][0].note[0]=60;endless_enable();assert(pat.trk[0].len==16);seq_trk=1;endless_record_toggle();assert(!pat.trk[1].len&&pat.trk[0].len==16);endless_arrow(1);endless_finish();endless_record_toggle();assert(pat.trk[1].len==1);endless_finish();
 reset();endless_record_toggle();for(unsigned i=0;i<256;i++)endless_arrow(1);assert(pat.trk[0].len==256);key(1,60);key(0,60);assert(pat.trk[0].len==256);endless_finish();
 /* Focus-safe recording release. */
 reset();endless_record_toggle();key(1,60);seq_trk=1;endless_poll();key(0,60);assert(!kb_press[60].count&&!end_count);
 /* Global restart includes legato note-ons, all ANALOG and fixed DRUM-X.
  * 3/5/7-step tracks stay independent after the common first boundary. */
 reset();for(unsigned t=0;t<4;t++)phrase(t,t==0?3:t==1?5:t==2?7:4);
 obv_drums_preset(&live_p[3],0);drum_pattern_set(&pat,3,0,1);endless_enable();endless_turn(3,-1);
 key(1,48);for(unsigned t=0;t<4;t++)assert(ui_seq_pos[t]==0);render(100);
 key(1,50);for(unsigned t=0;t<4;t++)assert(ui_seq_pos[t]==0);
 assert(end_transpose[0]==2&&end_transpose[1]==2&&end_transpose[2]==2&&end_transpose[3]==0);
 unsigned wrapped=0,previous=ui_seq_pos[0];
 for(unsigned i=0;i<10000&&!wrapped;i++){memset(audio,0,sizeof audio);engine_audio(audio,CTL);if(previous==2&&ui_seq_pos[0]==0){wrapped=1;assert(ui_seq_pos[1]==3&&ui_seq_pos[2]==3&&ui_seq_pos[3]==3);}previous=ui_seq_pos[0];}
 assert(wrapped);
 key(0,48);assert(ctl_seq_run&&end_held==1);render(5);assert(drum_stamp[3][0]);
 key(0,50);assert(!ctl_seq_run&&!end_held);for(unsigned t=0;t<4;t++)assert(ui_seq_pos[t]==0);
 endless_turn(3,1);key(1,48);key(0,48);assert(ctl_seq_run);render(800);
 for(unsigned t=0;t<4;t++)assert(ui_seq_pos[t]<pat.trk[t].len);assert(pat.trk[0].len==3&&pat.trk[1].len==5&&pat.trk[2].len==7);
 /* Drum focus preserves mode/HOLD and returning to ANALOG restores ENDLESS. */
 seq_trk=3;obv_context_refresh();assert(page==PG_DSEQ&&end_mode&&end_global_hold);seq_trk=1;obv_context_refresh();assert(page==PG_SEQ&&seq_view==SV_ENDLESS&&end_mode);
 /* K5 in ENDLESS changes BPM, never pattern/queue/steps. */
 unsigned saved=set.pat;set.tempo=120;host_enc[ENC_SELECT]=7;panel_encoders();assert(set.tempo==127&&ctl_tempo==127&&set.pat==saved&&!pat_queue);
 host_enc[ENC_SELECT]=1000;panel_encoders();assert(set.tempo==240);host_enc[ENC_SELECT]=-1000;panel_encoders();assert(set.tempo==40);
 /* Real slot roundtrip and E1 migration choose saved focus HOLD globally. */
 endless_stop();render(2);pattern_focus_set(&pat,1);mx_from_set(&psnd[0].mx);psnd[0].magic=PSN2_MAGIC;assert(!pattern_slot_save(15,&pat,&psnd[0]));pattern_t loaded;psnd_t snd;assert(!pattern_slot_load(15,&loaded,&snd));pat=loaded;endless_recall();assert(end_mode&&end_global_hold);for(unsigned t=0;t<4;t++)assert(endless_enabled(&pat.trk[t]));
 pat_init(&pat);pat.trk[1].pad[0]=0xD0;pat.trk[1].pad[1]=80;pattern_focus_set(&pat,1);endless_recall();assert(end_mode&&!end_global_hold&&pat.trk[1].pad[1]==80);for(unsigned t=0;t<4;t++)assert(!endless_hold(&pat.trk[t])&&pat.trk[t].len==16);
 /* Soft 1/16 scheduling, no premature transposition, no master phase reset.
  * Native CTL ticks resolve early, late and silent HOLD-OFF gaps. */
 reset();for(unsigned t=0;t<4;t++)phrase(t,8);endless_enable();ctl_tempo=120;
 key(1,48);assert(end_clock_valid&&end_global_transpose==0);
 while(end_clock_phase<quarter_len()/8u){memset(audio,0,sizeof audio);engine_audio(audio,CTL);}
 unsigned before_phase=end_clock_phase;panel_note_in(1,50,100);assert(end_pending&&end_global_transpose==0);
 unsigned waited=0;while(end_pending&&waited<quarter_len()/4u+CTL){memset(audio,0,sizeof audio);engine_audio(audio,CTL);waited+=CTL;}
 assert(!end_pending&&end_global_transpose==2&&waited>CTL);assert(end_clock_phase>=before_phase&&end_clock_phase<quarter_len()/4u+2*CTL);
 for(unsigned t=0;t<4;t++)assert(ui_seq_pos[t]==0);
 unsigned phase=end_clock_phase;panel_note_in(1,52,100);engine_audio(audio,CTL);assert(!end_pending&&end_global_transpose==4&&end_clock_phase>=phase);
 endless_turn(3,-1);key(0,48);key(0,50);key(0,52);assert(!ctl_seq_run&&end_clock_valid);
 for(unsigned i=0;i<20;i++)engine_audio(audio,CTL);
 panel_note_in(1,55,100);assert(end_pending&&!ctl_seq_run);panel_note_in(0,55,0);engine_audio(audio,CTL);assert(!end_pending&&!ctl_seq_run);
 for(unsigned i=0;i<quarter_len()/CTL+2;i++)engine_audio(audio,CTL);assert(!end_clock_valid);
 seq_beat_n=3;key(1,48);assert(ctl_seq_run&&!end_pending&&end_clock_phase<FS/50u&&ui_seq_bar_beat==0);key(0,48);
 /* External tempo drives the same launch grid and explicit STOP cancels wait. */
 ctl_ext_quarter=FS;key(1,48);while(end_clock_phase<FS/8u)engine_audio(audio,CTL);
 panel_note_in(1,50,100);assert(end_pending);endless_stop();engine_audio(audio,CTL);assert(!end_pending&&!ctl_seq_run&&!end_clock_valid);ctl_ext_quarter=0;
 /* E4: recording keeps the part octave; performance roots never inherit it. */
 for(unsigned t=0;t<4;t++)for(int oct=-2;oct<=2;oct++){
  reset();seq_trk=t;keyboard_physical=1;patch_keyboard_octave_set(&live_p[t],oct);endless_record_toggle();
  key(1,48);key(0,48);assert(pat.st[t][0].note[0]==48+12*oct);endless_finish();
  key(1,48);assert(end_global_transpose==0);key(0,48);key(1,50);assert(end_global_transpose==2);key(0,50);
  seq_trk=(t+1)%4;patch_keyboard_octave_set(&live_p[seq_trk],-oct);key(1,48);assert(end_global_transpose==0);key(0,48);
  keyboard_physical=0;key(1,60);assert(end_global_transpose==12);key(0,60);endless_stop();render(1);
 }
 reset();endless_enable();
 /* Unchanged display is idle; header redraw follows K5 tempo. */
 page=PG_SEQ;seq_view=SV_ENDLESS;seq_trk=0;toast=0;endless_draw(1);unsigned pix=host_lcd_pixels,blits=host_lcd_blits;for(unsigned i=0;i<1000;i++)endless_draw(0);assert(host_lcd_pixels==pix&&host_lcd_blits==blits);set.tempo++;endless_draw(0);assert(host_lcd_pixels-pix==240*44);
 /* Actual RGB565 frame: all parts retain their pad colour, including red A;
  * recording turns digits white while its badge remains unambiguously red. */
 end_pending=0;end_mode=1;
 for(unsigned t=0;t<4;t++){seq_trk=t;ui_trk=t;end_record=0;endless_draw(1);assert(fb[52*240+40]==C_TRKC[t]);char path[64];snprintf(path,sizeof path,"build/endless-play-%u.ppm",t);write_ppm_as(path);end_record=t+1;endless_draw(0);assert(fb[52*240+40]==RGB(255,255,255));assert(fb[1*240+195]==C_RED);}
 seq_trk=0;end_record=0;
 pat.trk[0].len=16;seq_cur[0]=6;pat.st[0][6].note[0]=60;pat.st[0][6].note[1]=64;pat.st[0][6].note[2]=67;ctl_seq_run=1;ui_seq_pos[0]=6;set.tempo=120;endless_draw(1);write_ppm_as("build/endless-play.ppm");end_record=1;end_pos=7;ctl_seq_run=0;endless_draw(0);write_ppm_as("build/endless-rec.ppm");
 puts("PASS E4: octave-aware recording/fixed performance root, colors/white REC, soft clock grid, independent recording, global HOLD/last-key stop/PLAY reset, legato global retrigger, 3/5/7-step clocks, fixed DRUM-X, focus persistence, exclusive K5 tempo, unchanged storage/E1 migration and partial display.");
}
