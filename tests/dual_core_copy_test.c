/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(void){
 uint32_t aa[260],bb[260];uint8_t *a=(uint8_t*)aa,*b=(uint8_t*)bb;
 for(unsigned sa=0;sa<4;sa++)for(unsigned da=0;da<4;da++)for(unsigned n=0;n<=1000;n++){
  for(unsigned i=0;i<sizeof aa;i++){a[i]=(uint8_t)(i*31u+n);b[i]=0xa5;}
  dc_copy(b+da,a+sa,n);assert(!memcmp(b+da,a+sa,n));
  for(unsigned i=0;i<da;i++)assert(b[i]==0xa5);
  for(unsigned i=da+n;i<sizeof bb;i++)assert(b[i]==0xa5);
 }
 puts("PASS: aligned/unaligned word copies, all tail lengths, 0..1000 bytes, guards intact.");return 0;
}
