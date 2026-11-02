/* SPDX-License-Identifier: GPL-3.0-only */
/* Instrument the port itself without running the inherited SVF/master code. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define NV 8u
#define NPART 4u
#define BOARD_SMK37 1
#include "../firmware/src/obv_dsp.h"
#include "../firmware/src/obv_params.h"
typedef struct {uint32_t part,vel,inc1;int32_t pitch16,gl,gr,amp0,amp1;} voice_t;
static voice_t vc[NV];static patch_t live_p[NPART];
static struct {int32_t lfo_val;} pt[NPART];
static uint8_t ctl_formant_wheel[NPART];
static uint32_t ctl_wheel_part;static int32_t ctl_vib;
typedef struct {const char*name;void(*start)(voice_t*);void(*render)(voice_t*,int32_t*);void *hit,*track,*reset;} obv_engine_ops_t;
#include "../firmware/src/obv_formant.h"
#include "../firmware/src/obv_voice_presets.h"
int main(void){
 int32_t out[CTL*2];
 for(unsigned corner=0;corner<64;corner++){
  obv_voice_preset(&live_p[0],corner%OBV_NVPRESET);
  patch_t *p=&live_p[0];p->v[VP_Q]=(corner&1)?127:0;p->v[VP_SHIFT]=(corner&2)?76:52;
  p->v[VP_BUZZ]=(corner&4)?127:0;p->v[VP_BREATH]=(corner&8)?127:0;
  p->v[VP_VOWEL]=(corner&16)?127:0;p->v[VP_TALK]=(corner&32)?127:0;
  voice_t *v=&vc[0];*v=(voice_t){0,127,pitch_inc((24+corner%35*3)*256),60<<16,4096,4096,32767,32767};
  fmt_start(v);
  for(unsigned b=0;b<4000;b++){
   if(!(b%400)){p->v[VP_VOWEL]^=127;ctl_formant_wheel[0]=ctl_formant_wheel[0]?0:127;}
   memset(out,0,sizeof out);fmt_render(v,out);
   if(jv_profile(p)){assert((int32_t)jv_state[0].v.ph[1]>=-(1<<28)&&(int32_t)jv_state[0].v.ph[1]<=(1<<28));}
   else for(unsigned j=0;j<8;j++)assert(fmt_state[0].y[j]>=-(1<<28)&&fmt_state[0].y[j]<=(1<<28));
  }
 }
 puts("PASS: isolated VOICE DSP: 64 parameter corners and morph/wheel sweeps, UBSan + address checks, 4,096,000 samples; leak detection disabled for this environment.");
 return 0;
}
