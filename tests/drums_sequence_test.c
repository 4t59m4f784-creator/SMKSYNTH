/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
static unsigned triggers[128];
static void drum_trace(int c,uint32_t t,uint32_t n){if(c=='S'&&t==0&&n<128)triggers[n]++;}
#define TRACE(c,t,n) drum_trace(c,t,n)
#define main obv_host_main
#ifndef OBV_TEST_SOURCE
#define OBV_TEST_SOURCE "../firmware/src/obervave.c"
#endif
#include OBV_TEST_SOURCE
#undef main
#include <assert.h>
int main(int argc,char **argv){
 int32_t audio[HALF_FRAMES*2];uint64_t energy=0;FILE *f=argc>1?fopen(argv[1],"wb"):NULL;
 if(argc>1)assert(f);
 engine_init();for(unsigned t=0;t<4;t++){trk_load(t,0);set.mix_lvl[t]=100;}
 set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();pat_init(&pat);
 obv_drums_preset(&live_p[0],argc>2?(unsigned)atoi(argv[2]):0);
 for(unsigned b=0;b<3;b++){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}
 pat.trk[0].transp=12; /* The map stays fixed; the saved melodic field is retained. */
 pat.trk[0].swing=58;
 for(unsigned s=0;s<16;s++){
  step_t *x=&pat.st[0][s];unsigned j=0;x->note[j++]=42;x->vel=s%4?83:115;
  if(s%4==0)x->note[j++]=36;
  if(s%8==4){x->note[j++]=38;x->note[j++]=39;}
  if(s==3)x->note[j++]=37;if(s==7)x->note[j++]=45;if(s==11)x->note[j++]=56;
  if(s==14)x->note[0]=46;
 }
 /* Consecutive tied steps still retrigger a one-shot drum. */
 pat.st[0][0].gate=pat.st[0][1].gate=GATE_TIE;
 pat.st[0][9].ratch=2;pat.st[0][9].nudge=20;
 pat.st[0][15].chance=0;
 ctl_seq_run=1;
 unsigned blocks=(44100u*8u+255u)/256u; /* four bars at 120 BPM */
 for(unsigned b=0;b<blocks+350;b++){
  if(b==blocks)ctl_seq_run=0;
  memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);
  for(unsigned i=0;i<HALF_FRAMES*2;i++)energy+=(int64_t)audio[i]*audio[i];
  if(f)fwrite(audio,1,sizeof audio,f);
 }
 if(f)fclose(f);
 for(unsigned l=0;l<8;l++)assert(triggers[DRUM_NOTE[l]]>0);
 assert(triggers[42]>60);assert(!triggers[48]);assert(energy>1000000);
 assert(pat.trk[0].transp==12);assert(!ctl_seq_run);
 for(unsigned i=0;i<NV;i++)assert(!vc[i].gate);
 printf("PASS: all eight instruments across native steps, simultaneous hits (four-note limit), swing/ratchet/nudge, tie retrigger, fixed drum map, stop; nonzero energy=%llu.\n",(unsigned long long)energy);
 return 0;
}
