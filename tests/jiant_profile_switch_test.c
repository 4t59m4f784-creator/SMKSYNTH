/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}
int main(void){engine_init();for(unsigned p=0;p<4;p++)trk_load(p,6);pat_init(&pat);mix_apply();fx_sync();
 obv_voice_preset(&live_p[0],0);live_p[0].v[OBV_ENGINE_TAG_AT+3]=1;render();render();note_event_p(0,1,60,100);for(unsigned b=0;b<20;b++)render();
 obv_voice_preset(&live_p[0],0);note_event_p(0,1,64,100);for(unsigned b=0;b<8;b++)render();assert(obv_engine_track[0].phase==OBV_ENGINE_READY);
 unsigned found=0;for(unsigned v=0;v<NV;v++)if(vc[v].part==0&&vc[v].gate){assert(jv_voice_profile[v]==2);assert(vc[v].note==64);found++;}assert(found==1);
 live_p[0].v[OBV_ENGINE_TAG_AT+3]=1;note_event_p(0,1,67,100);for(unsigned b=0;b<8;b++)render();found=0;
 for(unsigned v=0;v<NV;v++)if(vc[v].part==0&&vc[v].gate){assert(jv_voice_profile[v]==1);assert(vc[v].note==67);found++;}assert(found==1);
 puts("PASS: legacy/new VOICE profile switches fade, flush old cards and retain queued notes for the destination profile.");return 0;}
