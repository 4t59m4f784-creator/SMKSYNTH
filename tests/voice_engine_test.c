/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}
static uint64_t energy(void){uint64_t e=0;for(unsigned i=0;i<HALF_FRAMES*2;i++)e+=(int64_t)audio[i]*audio[i];return e;}
int main(int argc,char **argv){
 engine_init();for(unsigned t=0;t<4;t++)trk_load(t,0);for(unsigned t=0;t<4;t++){set.mix_lvl[t]=100;set.mix_cho[t]=set.mix_dly[t]=set.mix_rev[t]=0;}set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();pat_init(&pat);pf_init();
 patch_t legacy=live_p[0];assert(obv_patch_engine(&legacy)==OBV_ANALOG);
 for(unsigned p=0;p<OBV_NVPRESET;p++){
  patch_t v;obv_voice_preset(&v,p);assert(obv_patch_engine(&v)==OBV_VOICE&&sizeof v==80);
  for(unsigned j=0;j<NPARAM;j++)assert(v.v[j]<=PDEF[j].max);
 }
 obv_voice_preset(&live_p[0],0);assert(obv_engine_request(0,OBV_VOICE));render();render();
 note_event_p(0,1,60,110);uint64_t e=0;for(unsigned b=0;b<400;b++){render();e+=energy();}assert(e>10000000);
 /* Hardware mod wheel controls morph, not the fundamental's increment. */
 ctl_formant_wheel[0]=0;ctl_wheel_part=0;ctl_vib=0;render();unsigned c=0;while(c<NV&&(!vc[c].gate||vc[c].part!=0))c++;assert(c<NV);
 uint32_t pitch=vc[c].inc1;int32_t pos=fmt_state[c].pos;
 live_p[0].v[P_VINTAGE]=0;live_p[0].v[P_LFO_PITCH]=0;
 ctl_formant_wheel[0]=127;ctl_vib=127;for(unsigned b=0;b<100;b++)render();assert(fmt_state[c].pos>pos+10000);
 uint32_t moved=vc[c].inc1;assert(moved>pitch*99u/100u&&moved<pitch*101u/100u);
 ctl_wheel_part=1;for(unsigned b=0;b<100;b++)render();assert(fmt_state[c].pos>30000); /* VOICE morph stays latched after focus changes */
 ctl_formant_wheel[0]=0;ctl_wheel_part=0;ctl_vib=0;ctl_wheel_bend=4096;
 for(unsigned b=0;b<100;b++)render();assert(vc[c].inc1>moved*105u/100u&&vc[c].inc1<moved*107u/100u);
 ctl_wheel_bend=0;for(unsigned b=0;b<100;b++)render();
 /* All four sends receive VOICE audio; zeroing them clears send input while
  * shared effect tails are allowed to continue. */
 ctl_mix_cho[0]=ctl_mix_dly[0]=ctl_mix_rev[0]=ctl_mix_drv[0]=127;render();
 uint64_t sends[4]={0};for(unsigned i=0;i<HALF_FRAMES;i++){
  sends[0]+=(uint32_t)(snd_c[i]<0?-snd_c[i]:snd_c[i]);sends[1]+=(uint32_t)(snd_d[i]<0?-snd_d[i]:snd_d[i]);
  sends[2]+=(uint32_t)(snd_r[i]<0?-snd_r[i]:snd_r[i]);sends[3]+=(uint32_t)(snd_v[i]<0?-snd_v[i]:snd_v[i]);}
 for(unsigned i=0;i<4;i++)assert(sends[i]>0);
 ctl_mix_cho[0]=ctl_mix_dly[0]=ctl_mix_rev[0]=ctl_mix_drv[0]=0;render();
 for(unsigned i=0;i<HALF_FRAMES;i++)assert(!snd_c[i]&&!snd_d[i]&&!snd_r[i]&&!snd_v[i]);
 /* Native sustain, release, pitch bend and glide remain owned by the melodic card. */
 ctl_sustain=1;note_event_p(0,0,60,0);render();assert(vc[c].gate);
 ctl_sustain=0;for(unsigned b=0;b<2000;b++)render();assert(vc[c].ae.stage==E_IDLE);
 /* Stress all vowel/Q/shift/breath corners over low/high notes. */
 for(unsigned j=0;j<32;j++){
  obv_voice_preset(&live_p[0],2);live_p[0].v[VP_VOWEL]=(j&1)?127:0;
  live_p[0].v[VP_Q]=(j&2)?127:0;live_p[0].v[VP_SHIFT]=(j&4)?76:52;
  live_p[0].v[VP_BREATH]=(j&8)?127:0;live_p[0].v[VP_BUZZ]=(j&16)?127:0;
  live_p[0].v[P_MODE]=M_POLY;note_event_p(0,1,24+j*3,127);
  for(unsigned b=0;b<60;b++){render();for(unsigned i=0;i<HALF_FRAMES*2;i++)assert(audio[i]>=-32768&&audio[i]<=32767);}
  note_event_p(0,0,24+j*3,0);render();
 }
 ctl_panic=1;for(unsigned b=0;b<2000;b++)render();
 obv_voice_preset(&live_p[0],2);live_p[0].v[P_GLIDE]=100;live_p[0].v[P_VINTAGE]=0;
 note_event_p(0,1,60,100);for(unsigned b=0;b<1000;b++)render();
 c=0;while(c<NV&&(!vc[c].gate||vc[c].part))c++;assert(c<NV);uint32_t low=vc[c].inc1;
 note_event_p(0,1,72,100);render();assert(vc[c].inc1>low&&vc[c].inc1<low*19u/10u);
 ctl_panic=1;for(unsigned b=0;b<2000;b++)render();
 /* Mixed ANALOG/VOICE share the original 8-card allocator, not a second pool. */
 for(unsigned t=0;t<4;t++){if(t&1){obv_voice_preset(&live_p[t],0);assert(obv_engine_request(t,OBV_VOICE));}else{live_p[t]=legacy;assert(obv_engine_request(t,OBV_ANALOG));}}
 render();render();for(unsigned t=0;t<4;t++)note_event_p(t,1,48+t*7,110);
 for(unsigned b=0;b<200;b++)render();
 unsigned mask=0;for(unsigned v=0;v<NV;v++)if(vc[v].gate)mask|=1u<<vc[v].part;assert(mask==15);
 for(unsigned i=0;i<4;i++)assert(host_write_count[i]==0);
 printf("PASS: VOICE audio, 8 typed presets, wheel morph targeting, sustain/release, bend/glide, four sends, 32 extreme parameter combinations, 4-track mixed allocation, no flash writes. state=%zu bytes; eight cards remain a host limit, not safe hardware polyphony.\n",sizeof fmt_state);
 if(argc>1){
  /* Audition raw stereo int32 output. Start fresh in a new process per preset. */
  FILE *f=fopen(argv[1],"wb");assert(f);
  ctl_panic=1;for(unsigned b=0;b<2000;b++)render();
  for(unsigned t=0;t<4;t++){ctl_mix_lvl[t]=t?0:100;ctl_mix_cho[t]=ctl_mix_dly[t]=ctl_mix_rev[t]=0;}
  unsigned preset=argc>2?(unsigned)atoi(argv[2]):0;obv_voice_preset(&live_p[0],preset<8?preset:0);
  if(preset==8){
   patch_from_factory(&live_p[0],&FACTORY[6]);live_p[0].v[P_ARP_MODE]=A_OFF;
   obv_voice_preset(&live_p[1],0);obv_voice_preset(&live_p[2],2);
   ctl_mix_lvl[1]=80;ctl_mix_lvl[2]=75;
  }
  if(preset==9){
   live_p[0].v[P_CHO_MIX]=75;live_p[0].v[P_CHO_DEPTH]=85;
   live_p[0].v[P_REV_MIX]=65;live_p[0].v[P_REV_SIZE]=95;
   live_p[0].v[P_DLY_MIX]=35;fx_sync();
   ctl_mix_cho[0]=95;ctl_mix_rev[0]=90;ctl_mix_dly[0]=30;
  }
  obv_engine_request(0,obv_patch_engine(&live_p[0]));render();render();
  if(preset==8)assert(obv_engine_track[0].active==OBV_ANALOG&&obv_engine_track[1].active==OBV_VOICE&&obv_engine_track[2].active==OBV_VOICE);
  for(unsigned b=0;b<1100;b++){
   if(preset==8){
    if(b==0){note_event_p(0,1,36,100);note_event_p(1,1,60,100);note_event_p(1,1,64,100);note_event_p(1,1,67,100);note_event_p(2,1,72,100);}
    if(b==240){note_event_p(0,0,36,0);note_event_p(0,1,43,100);note_event_p(2,1,74,100);}
    if(b==480){note_event_p(0,0,43,0);note_event_p(0,1,41,100);note_event_p(2,1,76,100);}
    if(b==700){note_event_p(0,0,41,0);note_event_p(1,0,60,0);note_event_p(1,0,64,0);note_event_p(1,0,67,0);note_event_p(2,0,72,0);note_event_p(2,0,74,0);note_event_p(2,0,76,0);}
    render();fwrite(audio,1,sizeof audio,f);continue;
   }
   if(b==0){note_event_p(0,1,60,105);if(preset!=2&&preset!=3){note_event_p(0,1,64,100);note_event_p(0,1,67,100);}}
   if(preset==2||preset==3){if(b==240)note_event_p(0,1,67,105);if(b==480)note_event_p(0,1,72,105);}
   if(b==700){note_event_p(0,0,60,0);note_event_p(0,0,64,0);note_event_p(0,0,67,0);note_event_p(0,0,72,0);}
   render();fwrite(audio,1,sizeof audio,f);
  }
  fclose(f);
 }
 return 0;
}
