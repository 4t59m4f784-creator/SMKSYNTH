/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <time.h>
static volatile int32_t sink;
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
int main(void){for(unsigned change=0;change<2;change++){
 super_state_t s[8]={0};osc_t c[8]={0};int32_t out[CTL];uint32_t seed=12;
 for(unsigned i=0;i<8;i++)c[i].ph=xorshift(&seed);
 double begin=now();for(unsigned k=0;k<30000;k++)for(unsigned v=0;v<8;v++){uint32_t inc=pitch_inc((80+v*3)*256+(change?k%128:0));super_render(&s[v],&c[v],inc,70,90,out,CTL);sink=out[k%CTL];}
 printf("%s %.6f\n",change?"pitch-modulated":"steady",now()-begin);
 }printf("SUPER-state %zu bytes\n",sizeof(super_state_t));return 0;}
