/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(void){engine_init();for(unsigned t=0;t<4;t++)trk_load(t,6);pat_init(&pat);mix_apply();fx_sync();pf_init();
 patch_t analog=live_p[0];set_param_in(&live_p[0],P_VOLUME,127);assert(live_p[0].v[P_VOLUME]==127&&obv_param_max(&live_p[0],P_VOLUME)==127);
 obv_voice_preset(&live_p[0],0);seq_trk=0;obv_context_refresh();
 assert(obv_param_max(&live_p[0],P_VOLUME)==40);set_param_in(&live_p[0],P_VOLUME,127);assert(live_p[0].v[P_VOLUME]==40);
 char label[16];param_value(label,P_VOLUME,40);assert(!strcmp(label,"100%"));param_value(label,P_VOLUME,20);assert(!strcmp(label,"50%"));param_value(label,P_VOLUME,127);assert(!strcmp(label,"100%"));
 /* Parameter normalization for continuous MIDI inputs spans the new complete range. */
 for(unsigned cc=0;cc<128;cc++){set_param_in(&live_p[0],P_VOLUME,(cc*obv_param_max(&live_p[0],P_VOLUME)+63)/127);assert(live_p[0].v[P_VOLUME]<=40);}
 /* Old program/automation bytes stay byte-for-byte stored, but not their unsafe gain. */
 live_p[0].v[P_VOLUME]=127;patch_t saved=live_p[0];block_globals(&pt[0]);assert(obv_voice_volume(&live_p[0])==40);assert(!memcmp(&saved,&live_p[0],sizeof saved));
 voice_t*v=&vc[0];*v=(voice_t){0};v->part=0;v->note=60;v->vel=127;v->target16=v->pitch16=60<<16;v->gate=1;v->gl=v->gr=4096;fmt_start(v);int32_t out[64];
 for(unsigned n=0;n<500;n++){jv_control(v,0);assert(jv_state[0].m.amp1<=5243);memset(out,0,sizeof out);fmt_render(v,out);jv_control(v,0);fmt_render(v,out+32);}
 assert(!memcmp(&saved,&live_p[0],sizeof saved));
 /* The old vocal renderer is protected too; ANALOG and DRUMS keep original ranges. */
 live_p[0].v[OBV_ENGINE_TAG_AT+3]=1;block_globals(&pt[0]);assert(pt[0].gain_q12==400);
 live_p[0]=analog;live_p[0].v[P_VOLUME]=127;block_globals(&pt[0]);assert(pt[0].gain_q12==(127*127>>2));
 obv_drums_preset(&live_p[0],0);assert(obv_param_max(&live_p[0],P_VOLUME)==127);set_param_in(&live_p[0],P_VOLUME,127);assert(live_p[0].v[P_VOLUME]==127);
 for(unsigned p=0;p<8;p++){obv_voice_preset(&live_p[0],p);assert(live_p[0].v[P_VOLUME]<=40);}
 for(unsigned b=0;b<4;b++)assert(host_write_count[b]==0);
 puts("PASS: VOICE-only editor/DSP ceiling, 0–100% contextual display, all MIDI positions, original raw bytes untouched, both vocal profiles bounded, calibrated presets, ANALOG/DRUMS ranges unchanged, no flash writes.");return 0;}
