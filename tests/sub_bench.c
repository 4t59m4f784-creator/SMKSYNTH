/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <time.h>
#include <stdio.h>
int main(int argc,char**argv){unsigned level=argc>1?atoi(argv[1]):0;engine_init();patch_from_factory(&live_p[0],&FACT_INIT);
#ifdef SUB_BENCH
 patch_sub_set(&live_p[0],0,0,level);
#endif
 live_p[0].v[P_VINTAGE]=0;live_p[0].v[P_VEL_AMP]=0;block_globals(&pt[0]);
 for(unsigned v=0;v<NV;v++){voice_start(&vc[v],0,48+v,100,0,0);voice_block(&vc[v],v);vc[v].amp0=vc[v].amp1=20000;}
 int32_t out[CTL*2]={0};clock_t start=clock();
 for(unsigned b=0;b<60000;b++)for(unsigned v=0;v<NV;v++){memset(out,0,sizeof out);voice_render(&vc[v],out);}
 printf("%u %.6f voice=%zu part=%zu worker=%zu checksum=%d\n",level,(double)(clock()-start)/CLOCKS_PER_SEC,sizeof(voice_t),sizeof(part_t),sizeof(dc_work),out[0]);
}
