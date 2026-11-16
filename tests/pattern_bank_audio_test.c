/* SPDX-License-Identifier: GPL-3.0-only */
/* Load externally supplied records. No user bank or musical data is embedded. */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void exact(const char *path,void *p,unsigned n){FILE*f=fopen(path,"rb");assert(f&&fread(p,1,n,f)==n&&fgetc(f)==EOF);fclose(f);}
int main(int argc,char **argv){
 assert(argc==5);engine_init();pat_init(&pat);exact(argv[1],&pat,sizeof pat);exact(argv[2],&psnd[8],sizeof psnd[8]);exact(argv[3],&set,sizeof set);
 assert(pat.magic==PAT_MAGIC&&psnd[8].magic==PSN2_MAGIC);snd_target(8,live_p);snd_meta(8);ctl_tempo=set.tempo;fx_sync();mix_apply();keyboard_pattern_gains(&pat);
#if OBV_DUAL_CORE
 dc_enabled=(unsigned)atoi(argv[4]);
#endif
 ctl_seq_run=1;ctl_seq_restart=1;unsigned maxv=0,maxs=0,maxd=0;
 for(unsigned b=0;b<30000;b++){
  int32_t out[2u*HALF_FRAMES]={0};app_audio(out,HALF_FRAMES);assert(fwrite(out,sizeof out,1,stdout)==1);
  unsigned v=0,s=0,d=0;for(unsigned i=0;i<NV;i++)if(vc[i].ae.stage!=E_IDLE||vc[i].gate){v++;s+=super_voice(i);}
  for(unsigned p=0;p<NPART;p++)for(unsigned l=0;l<8;l++)d+=!!drum_hit[p][l].core.active;
  if(v>maxv)maxv=v;if(s>maxs)maxs=s;if(d>maxd)maxd=d;
 }
 assert(fwrite(drum_hit,sizeof drum_hit,1,stdout)==1);
 fprintf(stderr,"PASS: externally supplied pattern; 174 seconds audio; peak melodic=%u SUPER=%u drum hits=%u\n",maxv,maxs,maxd);
 return 0;
}
