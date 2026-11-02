/* SPDX-License-Identifier: GPL-3.0-only. Replay exact old patch bytes on both source snapshots. */
#define main obv_host_main
#ifndef OBV_TEST_SOURCE
#define OBV_TEST_SOURCE "../firmware/src/obervave.c"
#endif
#include OBV_TEST_SOURCE
#undef main
int main(int argc,char**argv){patch_t presets[8];if(argc==1){for(unsigned p=0;p<8;p++)obv_voice_preset(&presets[p],p);fwrite(presets,1,sizeof presets,stdout);return 0;}
 FILE*f=fopen(argv[1],"rb");if(!f||fread(presets,1,sizeof presets,f)!=sizeof presets)return 2;fclose(f);
 engine_init();for(unsigned t=0;t<4;t++){trk_load(t,6);set.mix_lvl[t]=t?0:100;set.mix_cho[t]=set.mix_dly[t]=set.mix_rev[t]=set.mix_drv[t]=0;}set.m_vol=100;set.m_dj=64;mix_apply();fx_sync();pat_init(&pat);
 int32_t out[HALF_FRAMES*2];for(unsigned p=0;p<8;p++){ctl_panic=1;engine_audio(out,HALF_FRAMES);live_p[0]=presets[p];for(unsigned k=0;k<800;k++){if(k==4){note_event_p(0,1,60,100);note_event_p(0,1,64,100);note_event_p(0,1,67,100);}if(k==400){note_event_p(0,0,60,0);note_event_p(0,0,64,0);note_event_p(0,0,67,0);}memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);fwrite(out,1,sizeof out,stdout);}}
 return 0;}
