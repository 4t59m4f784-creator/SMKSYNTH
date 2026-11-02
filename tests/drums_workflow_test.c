/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}
int main(int argc,char **argv){
 engine_init();for(unsigned n=0;n<NPSECT;n++)psect[n].tag=PS_TAG;
 for(unsigned t=0;t<4;t++){trk_load(t,t);set.mix_lvl[t]=100;}set.m_vol=100;set.m_dj=64;set.tempo=120;
 fx_sync();mix_apply();pf_init();pat_init(&pat);layer=L_NONE;page=PG_ENGINE;seq_trk=0;step_held=-1;
 host_enc[ENC_KNOB1]=1;panel_encoders();host_enc[ENC_KNOB1]=1;panel_encoders();
 assert(obv_patch_engine(&live_p[0])==OBV_DRUMS);render();render();
 /* Selecting a part keeps the performance focused on its sound controls. */
 page=PG_PERFORM;trk_select(1);assert(page==PG_PERFORM);trk_select(0);assert(page==PG_PERFORM);
 char value[16];param_value(value,DP_MORPH,0);assert(!strcmp(value,"0"));
 assert(obv_page_param(PG_PERFORM,0)==DP_MORPH&&obv_page_param(PG_PERFORM,3)==DP_FOLD);
 page=PG_DSOUND;drum_ui_lane[0]=2;
 host_enc[ENC_KNOB2]=-1;panel_encoders();assert(live_p[0].v[DP_LEVEL[2]]==98);
 host_enc[ENC_KNOB3]=1;panel_encoders();assert(live_p[0].v[DP_DECAY[2]]==66);
 host_enc[ENC_KNOB4]=1;panel_encoders();assert(live_p[0].v[DP_PITCH[2]]==65);
 uint32_t dirty=0;pf_nav(4);rec=0;
 for(unsigned p=0;p<8;p++){host_qput(0,(int32_t)(p<<8|110));smk_pads(48,&dirty);render();host_qput(0,(int32_t)(p<<8));smk_pads(48,&dirty);}
 assert(drums_busy(0));assert(!vc[0].gate);assert(drum_ui_lane[0]==7&&dirty_page);
 /* Real-time drum pads retain hit mode during REC and never write tied tails. */
 drums_reset(0);pat_init(&pat);pat.trk[0].transp=12;rec=REC_READY;ctl_seq_run=0;fm1_ms=100;
 assert(smk_drums_hits());host_qput(0,110);smk_pads(48,&dirty);render();
 assert((drum_pattern_mask(&pat,0,0)&1u)&&seq_live(0,36));
 fm1_ms+=800;host_qput(0,0);smk_pads(48,&dirty);render();
 assert(!seq_live(0,36)&&!rec_open[36].t&&pat.st[0][1].note[0]==0);
 rec=REC_OFF;ctl_seq_run=0;render();
 /* PATCH/performance restores normal track pads and retains favourites. */
 pf_nav(0);page=PG_PERFORM;host_qput(0,(1<<8|100));smk_pads(48,&dirty);assert(seq_trk==1);
 seq_trk=0;page=PG_DSOUND;pf_snapshot(&pf_store.slot[0],0);pf_store.used=1;
 obv_analog_choose(0);pf_recall(0,0);render();render();assert(obv_engine_track[0].active==OBV_DRUMS);
 pat.lane[0][0].param=DP_DECAY[2];pat.lane[0][0].v[0]=77;seq_auto(0,0);assert(live_p[0].v[DP_DECAY[2]]==77);
 for(unsigned i=0;i<4;i++)assert(host_write_count[i]==0);
 /* Real existing storage layouts roundtrip the typed kit and per-lane edits. */
 prog_put(90,&live_p[0]);assert(!psect_save(1,&psect[1],sizeof psect[1]));
 memset(&psect[1],0,sizeof psect[1]);assert(!psect_load(1,&psect[1],sizeof psect[1]));
 patch_t saved;prog_get(90,&saved);assert(obv_patch_engine(&saved)==OBV_DRUMS&&saved.v[DP_DECAY[2]]==77);
 set.pat=2;snd_capture(2);assert(!slot_save(2,&pat,sizeof pat,&psnd[2],sizeof psnd[2]));slot_saved(2,&pat);
 obv_analog_choose(0);render();render();load_pattern(2);render();render();assert(obv_patch_engine(&live_p[0])==OBV_DRUMS);
 assert(!memcmp(&saved,&live_p[0],sizeof saved));
 if(argc>1){
  char file[512];page=PG_PERFORM;draw_header();draw_page();draw_scope();ui_leds(ui_vlevel,NV);
  snprintf(file,sizeof file,"%s-performance.ppm",argv[1]);write_ppm_as(file);
  page=PG_DKIT;draw_page();snprintf(file,sizeof file,"%s-kit.ppm",argv[1]);write_ppm_as(file);
  page=PG_DSOUND;draw_page();snprintf(file,sizeof file,"%s-sound.ppm",argv[1]);write_ppm_as(file);
  page=PG_ENGINE;draw_page();snprintf(file,sizeof file,"%s-setup.ppm",argv[1]);write_ppm_as(file);
 }
 puts("PASS: engine choice stays in PART SETUP; performance/track workflow, eight explicit DRUM PADS, normal performance pads, real-time drum REC/hold release, per-instrument edit/automation, favourite recall, kit/program/pattern storage.");
 return 0;
}
