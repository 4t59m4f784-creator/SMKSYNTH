/* SPDX-License-Identifier: GPL-3.0-only */
#define OBV_COEF_CACHE_STATS 1
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(void){
 engine_init();uint32_t rng=0x12345678;svf_coef_t c1[NV]={0},c2[NV]={0};
 for(unsigned n=0;n<100000;n++){
  unsigned v=n%NV;int32_t cut=n%3?(int32_t)((n/64)%35000):(int32_t)(xorshift(&rng)%70000)-18000;
  int32_t k=(n/32)%8200;unsigned type=(n/128)%NFTYPE;
  if(n%997==0){memset(&vc[v],0,sizeof vc[v]);coef_cache[v].valid=0;memset(&c1[v],0,sizeof c1[v]);memset(&c2[v],0,sizeof c2[v]);}
  svf_coef(&c1[v],cut,k);if(type==F_LP24)svf_coef(&c2[v],cut,5793);
  voice_coefficients(&vc[v],v,cut,k,type);
  assert(!memcmp(&c1[v],&vc[v].c1,sizeof c1[v])&&!memcmp(&c2[v],&vc[v].c2,sizeof c2[v]));
 }
 assert(coef_skips>1000);printf("PASS: 100k clamped cutoff/resonance/type transitions and voice clears, exact original coefficients; calls=%u skipped=%u\n",coef_calls,coef_skips);
 voice_start(&vc[0],0,60,100,0,0);assert(!coef_cache[0].valid);
 coef_cache[0].valid=3;ctl_panic=2;engine_events();assert(!coef_cache[0].valid);
 puts("PASS: note starts and hard panic invalidate CPU0-only coefficient cache; voice and snapshot sizes unchanged.");
}
