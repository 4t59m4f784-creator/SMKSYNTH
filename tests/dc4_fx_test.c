/* SPDX-License-Identifier: GPL-3.0-only */
/* Identical input/control stream in DC3 and the independent DC4 DSP object.
 * Emit each stage and all semantic state (never process-specific pointers). */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
#include <time.h>
static void emit(const void *p,size_t n){assert(fwrite(p,1,n,stdout)==n);}
#define E(x) emit(&(x),sizeof(x))
static void state(void){
 line_t *lines[]={&cho,&dl,&dr,&rv_pre,&rv_in[0],&rv_in[1],&rv_in[2],&rv_in[3],&rv_ap1,&rv_d1,&rv_ap2,&rv_d2,&rv_ap3,&rv_d3,&rv_ap4,&rv_d4};
 for(unsigned i=0;i<sizeof(lines)/sizeof(*lines);i++){E(lines[i]->w);E(lines[i]->len);}
 E(cho_ph);E(rv_ph1);E(rv_ph2);E(dly_cur);E(dly_lpl);E(dly_lpr);E(rv_lp_in);E(rv_lpl);E(rv_lpr);E(rv_yl);E(rv_yr);
 E(dv_lp);E(dv_hold);E(dv_n);E(m_dj_was);E(m_c);E(m_fl);E(m_fr);E(m_env);E(m_g);uint32_t peak=ui_out_peak;E(peak);
}
int main(int argc,char **argv){
 unsigned bench=argc>1&&atoi(argv[1]),blocks=bench?60000u:12000u;
 patch_t p;patch_defaults(&p);fx_init();uint32_t rng=0x12345678u;
 int32_t out[2*HALF_FRAMES],cs[HALF_FRAMES],ds[HALF_FRAMES],rs[HALF_FRAMES],vs[HALF_FRAMES];
 clock_t t0=clock();
 for(unsigned k=0;k<blocks;k++){
  if(!(k%251u)){
   unsigned a=k/251u;
   for(unsigned i=P_CHO_RATE;i<=P_REV_MIX;i++)p.v[i]=(uint8_t)((a*43u+i*29u)&127u);
   ctl_dv_type=a%4;ctl_dv_amt=(a*17)&127;ctl_dv_tone=(a*23)&127;ctl_dv_mix=a%7?127:0;
   ctl_m_vol=(a*19)%128;ctl_m_dj=(a*11)%128;ctl_m_drive=a%3?(a*13)%128:0;ctl_m_comp=a%4?(a*37)%128:0;
  }
  /* Include extended silence to advance existing tails and eventual bypass. */
  if(!bench&&k==7000)p.v[P_CHO_MIX]=p.v[P_DLY_MIX]=p.v[P_REV_MIX]=0;
  if(!bench&&k>7000)p.v[P_CHO_MIX]=p.v[P_DLY_MIX]=p.v[P_REV_MIX]=0;
  fx_block(&p,14000u+(k%11000u));
  unsigned n=bench?HALF_FRAMES:(k%257u); /* zero, partial CTL and full halves */
  for(unsigned i=0;i<n;i++){
   int32_t x=(int32_t)(xorshift(&rng)>>16)-32768;
   out[2*i]=x/2;out[2*i+1]=-x/3;
   cs[i]=k<7000||bench?x:0;ds[i]=k<7000||bench?x/2:0;rs[i]=k<7000||bench?x/3:0;vs[i]=x;
  }
  fx_process(out,cs,ds,rs,n);if(!bench)emit(out,n*2*sizeof(*out));
  drive_process(out,vs,n);if(!bench)emit(out,n*2*sizeof(*out));
  master_process(out,n);if(!bench){emit(out,n*2*sizeof(*out));state();}
 }
 if(!bench)emit(fx_mem,sizeof fx_mem);
 fprintf(stderr,"PASS: FX/full-rate tails, stereo, drive types, master filter/compressor/limiter, parameter sweeps, sizes0..256, state equality fixture; CPU %.3fs%s\n",(double)(clock()-t0)/CLOCKS_PER_SEC,bench?" BENCH HOST ONLY":"");
}
