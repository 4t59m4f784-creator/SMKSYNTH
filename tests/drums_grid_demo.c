/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(int argc,char **argv){
 if(argc!=2)return 1;FILE *f=fopen(argv[1],"wb");assert(f);
 engine_init();for(unsigned t=0;t<4;t++){trk_load(t,0);set.mix_lvl[t]=100;}
 set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();pat_init(&pat);obv_drums_preset(&live_p[0],1);
 int32_t out[HALF_FRAMES*2];for(unsigned i=0;i<3;i++){memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);}
 for(unsigned n=0;n<16;n++){
  unsigned mask=1u<<3;if(!(n%4))mask|=1u;if(n%8==4)mask|=2u|4u;
  if(n==6||n==14)mask=(mask&~(1u<<3))|(1u<<4);
  if(n==3||n==11)mask|=1u<<6;if(n==15)mask|=1u<<5;if(n==0||n==12)mask|=1u<<7;
  if(n==12)mask|=1u<<6; /* six simultaneous hits, beyond the old four-note cap */
  drum_pattern_set(&pat,0,n,mask);pat.st[0][n].vel=(uint8_t)(n%4?82:114);
 }
 pat.st[0][15].ratch=1;pat.st[0][3].nudge=10;ctl_seq_run=ctl_seq_restart=1;
 for(unsigned b=0;b<1729;b++){
  if(b==1379)ctl_seq_run=0;memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);fwrite(out,1,sizeof out,f);
 }fclose(f);return 0;
}
