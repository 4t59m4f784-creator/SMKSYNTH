/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}
int main(int argc,char **argv){
 engine_init();for(unsigned n=0;n<NPSECT;n++)psect[n].tag=PS_TAG;for(unsigned t=0;t<4;t++){trk_load(t,t+1);set.mix_lvl[t]=100;}set.m_vol=100;set.m_dj=64;set.tempo=120;
 fx_sync();mix_apply();pf_init();pat_init(&pat);step_held=-1;layer=L_NONE;
 for(unsigned n=0;n<NPROG;n++){patch_t p;prog_get(n,&p);assert(obv_patch_engine(&p)==OBV_ANALOG);}
 patch_t a=live_p[0];page=PG_ENGINE;seq_trk=0;
 host_enc[ENC_KNOB1]=1;panel_encoders();assert(obv_patch_engine(&live_p[0])==OBV_VOICE);
 /* A key arriving on the same audio half as the engine change is retained. */
 note_event_p(0,1,60,110);render();render();assert(obv_engine_track[0].active==OBV_VOICE&&nheld[0]==1);
 unsigned voice=NV;for(unsigned v=0;v<NV;v++)if(vc[v].part==0&&vc[v].gate)voice=v;
 assert(voice<NV&&obv_voice_engine[voice]==OBV_VOICE);
 host_enc[ENC_KNOB1]=-1;panel_encoders();assert(!memcmp(&a,&live_p[0],sizeof a));render();render();
 obv_voice_choose(0,2);render();render();assert(obv_patch_engine(&live_p[0])==OBV_VOICE);
 page=PG_VOWELS;host_enc[ENC_KNOB4]=1;uint8_t shift=live_p[0].v[VP_SHIFT];panel_encoders();assert(live_p[0].v[VP_SHIFT]==shift+1);
 set_param(VP_SHIFT,0);assert(live_p[0].v[VP_SHIFT]==52);set_param(VP_SHIFT,127);assert(live_p[0].v[VP_SHIFT]==76);
 assert(!strcmp(obv_voice_label(VP_VOWEL),"VOWEL"));
 /* Existing automation lanes retain VOICE parameters at the same parameter IDs. */
 pat.lane[0][0].param=VP_VOWEL;pat.lane[0][0].v[0]=95;seq_auto(0,0);assert(live_p[0].v[VP_VOWEL]==95);
 pf_snapshot(&pf_store.slot[0],0);pf_store.used|=1;assert(pf_matches(0,0));
 obv_patch_set_engine(&live_p[0],OBV_ANALOG);assert(!pf_matches(0,0));
 pf_recall(0,0);render();render();assert(obv_engine_track[0].active==OBV_VOICE);
 for(unsigned i=0;i<4;i++)assert(host_write_count[i]==0);
 /* Explicit save, read and compare the real host storage formats. */
 pf_dirty=1;pf_manual_write=1;pf_t=0;fm1_ms=5000;ctl_seq_run=0;
 ctl_panic=1;for(unsigned b=0;b<3000;b++)render();pf_save_poll();
 pf_init();assert(obv_patch_engine(&pf_store.slot[0].patch)==OBV_VOICE);
 prog_put(90,&live_p[0]);assert(!psect_save(1,&psect[1],sizeof psect[1]));
 memset(&psect[1],0,sizeof psect[1]);assert(!psect_load(1,&psect[1],sizeof psect[1]));
 patch_t saved;prog_get(90,&saved);assert(obv_patch_engine(&saved)==OBV_VOICE);
 set.pat=1;snd_capture(1);assert(!slot_save(1,&pat,sizeof pat,&psnd[1],sizeof psnd[1]));slot_saved(1,&pat);
 obv_analog_choose(0);render();render();load_pattern(1);render();render();assert(obv_engine_track[0].active==OBV_VOICE);
 assert(!memcmp(&saved,&live_p[0],sizeof saved));
 /* Queued pattern: first downbeat note uses the new engine, not the old one. */
 ctl_seq_run=0;obv_analog_choose(0);render();render();pat_init(&pat);pat_init(&pat_q);
 pat_q.st[0][0].note[0]=67;pat_q.st[0][0].gate=110;
 for(unsigned t=0;t<4;t++)snd_q[t]=live_p[t];obv_voice_preset(&snd_q[0],0);
 ctl_pat_swap=1;ctl_snd_swap=1;ctl_seq_run=1;ctl_seq_restart=1;render();render();
 assert(obv_engine_track[0].active==OBV_VOICE&&sr[0].nsnd==1);
 voice=NV;for(unsigned v=0;v<NV;v++)if(vc[v].part==0&&vc[v].gate&&vc[v].note==67)voice=v;
 assert(voice<NV&&obv_voice_engine[voice]==OBV_VOICE);
 /* Export native framebuffer renders for visual QA, not browser screenshots. */
 if(argc>1){
  char file[512];ctl_seq_run=0;page=PG_PERFORM;layer=L_NONE;draw_header();draw_page();draw_scope();ui_leds(ui_vlevel,NV);
  snprintf(file,sizeof file,"%s-performance.ppm",argv[1]);write_ppm_as(file);
  page=PG_VOWELS;draw_page();snprintf(file,sizeof file,"%s-vowels.ppm",argv[1]);write_ppm_as(file);
  page=PG_VTONE;draw_page();snprintf(file,sizeof file,"%s-tone.ppm",argv[1]);write_ppm_as(file);
  page=PG_ENGINE;draw_page();snprintf(file,sizeof file,"%s-engine.ppm",argv[1]);write_ppm_as(file);
 }
 puts("PASS: 99 legacy programs remain ANALOG, engine/preset controls, parked ANALOG, queued notes, contextual SHIFT, automation, typed favourite match/recall, explicit favourites/program/pattern roundtrip, queued pattern first-note routing.");
 return 0;
}
