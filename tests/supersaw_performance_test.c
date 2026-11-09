/* SPDX-License-Identifier: GPL-3.0-only */
/* Preserve the approved SS1 oscillator sample-for-sample while removing work.
 * Host wall times are comparative, not a claim about the keyboard deadline. */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
#include <time.h>
#define super_state_t ss1_state_t
#define super_curve ss1_curve
#define super_render ss1_render
#include "reference/supersaw/ss1.h"
#undef super_state_t
#undef super_curve
#undef super_render

static volatile int32_t sink;
static void same_state(const super_state_t *a, const ss1_state_t *b)
{
    assert(a->det_sm==b->det_sm && a->mix_sm==b->mix_sm && a->active==b->active);
    assert(!memcmp(a->side,b->side,sizeof a->side));
}
static void equivalent(void)
{
    for(int32_t x=-32768*6;x<=32768*6;x++)assert(super_div6(x)==x/6);
    for(int32_t x=-32512;x<=32512;x++)assert(super_div16(x)==x/16);
    uint32_t seed=1234567, blocks=0;
    for(unsigned note=0;note<128;note++) {
        super_state_t a={0};ss1_state_t b={0};osc_t ca={xorshift(&seed),0},cb=ca;
        int32_t x[CTL],y[CTL];
        for(unsigned k=0;k<256;k++) {
            unsigned det=k<32?64:k<64?0:k<96?127:xorshift(&seed)%128;
            unsigned mix=k<16?0:k<64?64:k<96?127:k<128?0:xorshift(&seed)%128;
            uint32_t inc=pitch_inc(note*256+(k>=128?xorshift(&seed)%256:0));
            unsigned n=1+k%CTL;
            ss1_render(&b,&cb,inc,det,mix,y,n);
            int silent=(k%11)==0;
            super_render(&a,&ca,inc,det,mix,silent?0:x,n);
            if(!silent)assert(!memcmp(x,y,n*sizeof *x));
            assert(!memcmp(&ca,&cb,sizeof ca));same_state(&a,&b);blocks++;
            /* Waveform off/on reinitializes the side phases just as in SS1. */
            if(k==64){a.active=0;b.active=0;}
        }
    }
    printf("PASS: %u paired blocks, all 128 MIDI notes, changing detune/mix/pitch, silent fast-forward, variable block sizes, phase/state and output bit-identical to SS1.\n",blocks);
}
static double now(void)
{
    struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;
}
static double bench(int old,unsigned voices,unsigned oscs,int modulated)
{
    super_state_t a[8][2]={0};ss1_state_t b[8][2]={0};osc_t c[8][2]={0};
    uint32_t seed=12345;int32_t out[CTL];
    for(unsigned v=0;v<voices;v++)for(unsigned o=0;o<oscs;o++)c[v][o].ph=xorshift(&seed);
    double start=now();
    for(unsigned k=0;k<12000;k++)for(unsigned v=0;v<voices;v++)for(unsigned o=0;o<oscs;o++) {
        uint32_t inc=pitch_inc((48+v*3+o)*256+(modulated?k%128:0));
        if(old)ss1_render(&b[v][o],&c[v][o],inc,64,64,out,CTL);
        else super_render(&a[v][o],&c[v][o],inc,64,64,out,CTL);
        sink=out[k%CTL];
    }
    return now()-start;
}
int main(void)
{
    equivalent();
    uint32_t seed=87654;
    for(uint32_t d=1;d<=65535;d++)for(unsigned k=0;k<24;k++){
        uint32_t n=k==0?0:k==1?0xFFFFFFFFu:k==2?d:k==3?d-1:xorshift(&seed);
        assert(super_fraction(n,d,super_reciprocal(d))==n/d);
    }
    for(unsigned note=0;note<128;note++){
        osc_t a={xorshift(&seed),0},b=a;super_plain_state_t state={0};int32_t out[CTL];
        for(unsigned k=0;k<256;k++){
            uint32_t inc=pitch_inc(note*256+k);int silent=k%11==0;
            super_plain_saw(&state,&a,inc,silent?0:out,CTL);
            for(unsigned i=0;i<CTL;i++){
                int32_t y=osc_step(&b,inc,inc>>15,0,OSC_SAW,-65536,-1);
                if(!silent)assert(out[i]==y);
            }
            assert(!memcmp(&a,&b,sizeof a));
        }
    }
    puts("PASS: 1572840 reciprocal quotients and 32768 companion-SAW blocks exact, including high notes and silent advance.");
    for(unsigned voices=4;voices<=8;voices+=4)for(unsigned oscs=1;oscs<=2;oscs++)for(int mod=0;mod<=1;mod++){
        double a=bench(1,voices,oscs,mod),b=bench(0,voices,oscs,mod);
        printf("HOST %u voices x %u SUPER, %s: SS1 %.4fs SS3 %.4fs ratio %.3f\n",voices,oscs,mod?"changing pitch":"steady pitch",a,b,b/a);
    }
    puts("Timing is a host microbenchmark of oscillators only; measure attack peaks and overruns on the SMK37.");
    return 0;
}
