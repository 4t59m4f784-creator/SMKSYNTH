/* SPDX-License-Identifier: GPL-3.0-only */
/* Same deterministic mixed-engine session through serial, pthread worker,
 * rejected dispatch and failed/partially-written private jobs. Compare PCM
 * and final drum state byte-for-byte across independently started processes. */
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
static pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
static pthread_t thread;
static void (*job_fn)(uint32_t);
static uint32_t job_arg;
static unsigned pending,done,quit,submitted,fail_mode,plain_only,normal_jobs,super_jobs;
#define DC_TEST_BACKEND 1
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void *worker(void *unused)
{
 (void)unused;pthread_mutex_lock(&lock);
 for(;;){
  while(!pending&&!quit)pthread_cond_wait(&cond,&lock);
  if(quit)break;
  void (*fn)(uint32_t)=job_fn;uint32_t arg=job_arg;
  pthread_mutex_unlock(&lock);fn(arg);pthread_mutex_lock(&lock);
  pending=0;done=1;pthread_cond_broadcast(&cond);
 }
 pthread_mutex_unlock(&lock);return 0;
}
#if OBV_DUAL_CORE
static int dc_host_run(void (*fn)(uint32_t),uint32_t arg)
{
 submitted++;
 for(unsigned v=0;v<NV;v++)if(arg&(1u<<v)){if(pt[vc[v].part].w1==W_SUPER)super_jobs++;else normal_jobs++;}
 if(fail_mode==1&&submitted%7u==0)return 0;
 pthread_mutex_lock(&lock);assert(!pending);job_fn=fn;job_arg=arg;done=0;pending=1;
 pthread_cond_broadcast(&cond);pthread_mutex_unlock(&lock);return 1;
}
static int dc_host_wait(void)
{
 pthread_mutex_lock(&lock);while(!done)pthread_cond_wait(&cond,&lock);pthread_mutex_unlock(&lock);
 if(fail_mode==2&&submitted%11u==0){
  /* The entire private result is corrupt/partial. Original state is intact. */
  memset(&dc_work,0xA5,sizeof dc_work);return -1;
 }
 return 0;
}
#endif
int main(int argc,char **argv)
{
 fail_mode=argc>1?(unsigned)atoi(argv[1]):0;
 plain_only=argc>2?(unsigned)atoi(argv[2]):0;
 pthread_create(&thread,0,worker,0);
#if OBV_DUAL_CORE
 dp_state=DP_RUNNING;dp_capture=1;
#endif
 engine_init();for(unsigned p=0;p<NPART;p++)trk_load(p,0);
 set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();pat_init(&pat);pf_init();
 for(unsigned b=0;b<1800;b++){
#if OBV_DUAL_CORE
  if(fail_mode==3)dc_enabled=(b/37u)%2u;
#endif
  if(b%200==0){
   ctl_panic=1;
   for(unsigned p=0;p<NPART;p++){
    if(p<2||b>=1400)obv_drums_preset(&live_p[p],(b/200+p)%NDRUMKIT);
    else{patch_from_factory(&live_p[p],&FACTORY[p==2?6:0]);live_p[p].v[P_ARP_MODE]=A_OFF;live_p[p].v[P_MODE]=M_POLY;
     patch_sub_set(&live_p[p],(b/200+p)&1,(b/400+p)&1,100);
     if(p==3&&!plain_only){live_p[p].v[P_O1_WAVE]=W_SUPER;}}
   }
  }
  if(b%9==0)for(unsigned p=0;p<2;p++)for(unsigned l=0;l<8;l++)
   note_event_p(p,1,DRUM_NOTE[l],30u+(b*13u+p*17u+l*7u)%98u);
  if(b%17==0)for(unsigned k=0;k<3;k++){note_event_p(2,1,48+b%12+k*4,90);note_event_p(3,1,60+b%12+k*4,100);}
  if(b%17==4){for(unsigned n=48;n<84;n++){note_event_p(2,0,n,0);note_event_p(3,0,n,0);}}
  if(b%31==0){live_p[0].v[DP_MORPH]=b%128;live_p[1].v[DP_TONE]=b%128;}
  if(b%13==0)for(unsigned p=2;p<4;p++)patch_sub_set(&live_p[p],b&1,(b>>1)&1,b%101);
  if(b%227==226)ctl_panic=1;
  int32_t out[HALF_FRAMES*2]={0};engine_audio(out,HALF_FRAMES);
  assert(fwrite(out,sizeof out,1,stdout)==1);
 }
 assert(fwrite(drum_hit,sizeof drum_hit,1,stdout)==1);
 assert(fwrite(vc,sizeof vc,1,stdout)==1);
 assert(fwrite(super_state,sizeof super_state,1,stdout)==1);
 assert(fwrite(super_plain_state,sizeof super_plain_state,1,stdout)==1);
#if OBV_DUAL_CORE
 assert(dc_jobs>1000);assert(normal_jobs>0);if(plain_only)assert(!super_jobs);else assert(super_jobs>0);if(fail_mode==1||fail_mode==2)assert(dc_fallbacks>0);
 fprintf(stderr,"PASS mode=%u jobs=%u fallbacks=%u private=%zu bytes normal=%u SUPER=%u plain_only=%u\n",fail_mode,dc_jobs,dc_fallbacks,sizeof dc_work,normal_jobs,super_jobs,plain_only);
#endif
 pthread_mutex_lock(&lock);quit=1;pthread_cond_broadcast(&cond);pthread_mutex_unlock(&lock);pthread_join(thread,0);
 return 0;
}
