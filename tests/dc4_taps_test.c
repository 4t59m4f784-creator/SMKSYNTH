/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t old_linear(const line_t*l,int32_t q){unsigned b=(uint32_t)q>>16;int32_t f=(q>>8)&255,a=line_read(l,b),c=line_read(l,b+1);return a+(((c-a)*f)>>8);}
static int32_t old_hermite(const line_t*l,int32_t q){unsigned b=(uint32_t)q>>16;int32_t f=(q>>8)&255,p0=line_read(l,b-1),p1=line_read(l,b),p2=line_read(l,b+1),p3=line_read(l,b+2);int32_t c1=(p2-p0)>>1,c2=p0-((5*p1)>>1)+2*p2-(p3>>1),c3=((p3-p0)>>1)+((3*(p1-p2))>>1);return p1+((f*(c1+((f*(c2+((f*c3)>>8)))>>8)))>>8);}
int main(void){
 fx_init();uint32_t r=1234567;for(unsigned i=0;i<FX_MEM_N;i++)fx_mem[i]=(int16_t)xorshift(&r);
 const unsigned f[]={0,1,127,128,254,255};
 for(unsigned w=0;w<CHO_N;w++){cho.w=w;for(unsigned b=1;b<CHO_N-1;b++)for(unsigned j=0;j<6;j++){int32_t q=(int32_t)((b<<16)|(f[j]<<8));assert(cho_read_q16_h(&cho,q)==old_hermite(&cho,q));}}
 const unsigned w[]={0,1,2,DLY_MAX/2,DLY_MAX-2,DLY_MAX-1};
 for(unsigned k=0;k<6;k++){dl.w=w[k];for(unsigned b=64;b<=DLY_MAX-4;b++)for(unsigned j=0;j<6;j++){int32_t q=(int32_t)((b<<16)|(f[j]<<8));assert(dly_read_q16(&dl,q)==old_linear(&dl,q));}}
 for(unsigned k=0;k<1000000;k++){dl.w=xorshift(&r)&(DLY_MAX-1);unsigned b=64+xorshift(&r)%(DLY_MAX-67);int32_t q=(int32_t)((b<<16)|(xorshift(&r)&65535));assert(dly_read_q16(&dl,q)==old_linear(&dl,q));}
 puts("PASS: every chorus write index/tap offset and fractional boundaries; all delay offsets at wrap boundaries plus 1M random fractional taps, exactly equal to DC3 interpolation.");
}
