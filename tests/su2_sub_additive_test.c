/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
#include <math.h>
static double reference[16384],sum[16384],subonly[16384];
static void capture(double*dst,unsigned sub,unsigned mainosc){
 memset(vc,0,sizeof vc);memset(pt,0,sizeof pt);memset(super_state,0,sizeof super_state);engine_init();for(unsigned t=0;t<NPART;t++)pt[t].p=&live_p[t];patch_from_factory(&live_p[0],&FACT_INIT);patch_t*p=&live_p[0];
 p->v[P_O1_WAVE]=W_PLS;p->v[P_O1_LVL]=mainosc?70:0;p->v[P_O2_LVL]=p->v[P_NOISE]=p->v[P_VINTAGE]=p->v[P_LFO_PITCH]=p->v[P_LFO_FLT]=p->v[P_LFO_AMP]=p->v[P_MOD_DEPTH]=0;
 p->v[P_FTYPE]=F_LP12;p->v[P_RESO]=0;p->v[P_DRIVE]=0;p->v[P_SPREAD]=0;p->v[P_FENV_AMT]=64;p->v[P_KEYTRK]=0;p->v[P_VEL_FLT]=p->v[P_VEL_AMP]=0;p->v[P_AA]=p->v[P_AD]=0;p->v[P_AS]=127;p->v[P_FA]=p->v[P_FD]=0;p->v[P_FS]=127;p->v[P_CUTOFF]=100;
 patch_sub_set(p,0,0,sub);ctl_bend=ctl_wheel_bend=0;ctl_tune8=0;
 for(unsigned i=0;i<4096;i++)block_globals(&pt[0]);voice_start(&vc[0],0,69,100,0,0);
 unsigned stored=0;for(unsigned b=0;b<3072;b++){int32_t out[2*CTL]={0};voice_block(&vc[0],0);voice_render(&vc[0],out);if(b>=2048)for(unsigned i=0;i<CTL;i++)dst[stored++]=out[2*i];}
 assert(stored==16384);
}
static double power(double*p,double freq){double x=0,y=0;for(unsigned i=0;i<16384;i++){double w=.5-.5*cos(2*3.141592653589793*i/16383),a=2*3.141592653589793*freq*i/FS;x+=p[i]*w*cos(a);y+=p[i]*w*sin(a);}return sqrt(x*x+y*y);}
int main(){pf_init();pat_init(&pat);capture(reference,0,1);capture(sum,100,1);capture(subonly,100,0);
 double main_before=power(reference,440),main_after=power(sum,440),sub=power(sum,220);double ratio=main_after/main_before;
 printf("OSC1 440 Hz after/before SUB: %.6f; SUB 220 Hz power %.1f; no mix normalization.\n",ratio,sub);assert(ratio>.98&&ratio<1.02&&sub>1000);
 double err=0,rms=0;for(unsigned i=0;i<16384;i++){double e=sum[i]-reference[i]-subonly[i];err+=e*e;rms+=sum[i]*sum[i];}printf("Linear additive residual RMS %.3f, signal RMS %.3f (fixed-point rounding included).\n",sqrt(err/16384),sqrt(rms/16384));assert(sqrt(err/(rms+1))<.02);
 puts("PASS: SUB adds its fundamental without attenuating the original oscillator; no extra voices.");
}
