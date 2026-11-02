/* SPDX-License-Identifier: GPL-3.0-only. Offline stage-level headroom measurement. */
#define main obv_host_main
#ifndef OBV_TEST_SOURCE
#define OBV_TEST_SOURCE "../firmware/src/obervave.c"
#endif
#include OBV_TEST_SOURCE
#undef main
#include <math.h>
int main(int argc,char**argv){
 unsigned kind=atoi(argv[1]),preset=atoi(argv[2]),volume=atoi(argv[3]),wet=atoi(argv[4]),notes=atoi(argv[5]),note=atoi(argv[6]);
 engine_init();for(unsigned t=0;t<4;t++){trk_load(t,6);set.mix_lvl[t]=t?0:100;set.mix_cho[t]=set.mix_dly[t]=set.mix_rev[t]=set.mix_drv[t]=0;}set.m_vol=100;set.m_dj=64;set.m_drive=set.m_comp=0;pat_init(&pat);
 if(kind)obv_voice_preset(&live_p[0],preset);else patch_from_factory(&live_p[0],&FACTORY[preset]);
 live_p[0].v[P_ARP_MODE]=A_OFF;live_p[0].v[P_VOLUME]=volume;live_p[0].v[P_VINTAGE]=0;live_p[0].v[P_GLIDE]=0;
 if(wet){set.mix_cho[0]=set.mix_dly[0]=set.mix_rev[0]=80;live_p[0].v[P_CHO_MIX]=70;live_p[0].v[P_DLY_MIX]=55;live_p[0].v[P_REV_MIX]=70;}
 mix_apply();fx_sync();int32_t out[HALF_FRAMES*2];for(unsigned b=0;b<4;b++)engine_audio(out,HALF_FRAMES);
 for(unsigned n=0;n<notes;n++)note_event_p(0,1,note+n*4,100);
 memset(audit_clip,0,sizeof audit_clip);memset(audit_peak,0,sizeof audit_peak);uint64_t energy=0,count=0;int peak=0;
 for(unsigned b=0;b<320;b++){memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);if(b>=192)for(unsigned i=0;i<HALF_FRAMES*2;i++){int x=out[i];energy+=(int64_t)x*x;count++;if(abs(x)>peak)peak=abs(x);}}
 printf("%u,%u,%u,%u,%u,%u,%.3f,%d,%llu,%llu,%d,%d\n",kind,preset,volume,wet,notes,note,sqrt((double)energy/count),peak,(unsigned long long)audit_clip[1],(unsigned long long)audit_clip[2],audit_peak[1],audit_peak[2]);
 return 0;}
