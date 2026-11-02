/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(void){engine_init();uint64_t checks=0;
 for(unsigned pre=0;pre<8;pre++)for(unsigned vi=0;vi<4;vi++){
  static const unsigned velocities[4]={1,64,100,127};unsigned vel=velocities[vi];
  obv_voice_preset(&live_p[0],pre);ctl_formant_wheel[0]=0;pt[0].bend8=ctl_tune8=0;pt[0].lfo_val=0;
  memset(&jv_state[0],0,sizeof jv_state[0]);vc[0]=(voice_t){0};voice_t*v=&vc[0];v->part=0;v->note=60;v->vel=vel;v->target16=v->pitch16=60<<16;v->gl=v->gr=4096;v->gate=1;jv_start(v);
  int32_t env=0,stage=1,amp=0;
  for(unsigned b=0;b<1600;b++){
   const uint8_t*p=live_p[0].v;int32_t before=amp;
   if(b==800){v->gate=0;stage=3;}
   if(stage==1){env+=(int32_t)JV_ENV_LIN[p[P_AA]];if(env>=(1<<24)){env=1<<24;stage=2;}}
   else if(stage==2){int32_t d=((int32_t)p[P_AS]<<17)-env;env+=(d>>16)*(int32_t)JV_ENV_EXP[p[P_AD]]+(int32_t)(((uint32_t)(d&65535)*JV_ENV_EXP[p[P_AD]])>>16);}
   else if(stage==3){env-=(env>>16)*(int32_t)JV_ENV_EXP[p[P_AR]]+(int32_t)(((uint32_t)(env&65535)*JV_ENV_EXP[p[P_AR]])>>16);if(env<(1<<12)){env=0;stage=0;}}
   amp=((env>>9)*(vel*258))>>15;
   uint32_t volume=obv_voice_volume(&live_p[0]);amp=(int32_t)((int64_t)amp*volume*volume/10000);
   jv_control(v,0);assert(jv_state[0].v.env==env);assert(jv_state[0].m.amp0==before&&jv_state[0].m.amp1==amp);checks++;
   int32_t audio[64]={0};jv_render(v,audio);jv_control(v,0);assert(jv_state[0].v.env==env);jv_render(v,audio+32);
  }
 }
 printf("PASS: %llu reference 32-frame envelope/velocity ticks with two 16-frame slices per tick.\n",(unsigned long long)checks);return 0;
}
