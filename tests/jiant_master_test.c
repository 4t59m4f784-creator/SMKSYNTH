/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
#include "reference/jiant/master_fixture.h"
int main(void){uint32_t rnd=123;uint64_t n=0;jmx_reset();
 for(unsigned k=0;k<4000;k++){
  int32_t out[64],want[64];
  for(unsigned i=0;i<32;i++){int32_t x=((int32_t)(xorshift(&rnd)>>16)-32768)*(k<1000?1:k<2000?4:k<3000?8:0);int32_t y=x/2;out[2*i]=ref_mix_l[i]=x;out[2*i+1]=ref_mix_r[i]=y;}
  ref_lev_block(32);for(unsigned i=0;i<32;i++){int32_t l=ref_mix_l[i],r=ref_mix_r[i];ref_master_out(&l,&r);want[2*i]=l;want[2*i+1]=r;}
  jmx_process(out,32,4096);for(unsigned i=0;i<64;i++){assert(out[i]==want[i]);n++;}
 }
 printf("PASS: %llu exact stereo samples against JIANT default DC/leveler/limiter/softclip, low level, overloaded mix and silence recovery.\n",(unsigned long long)n);return 0;
}
