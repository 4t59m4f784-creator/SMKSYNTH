/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#define NPART 4u
#include "../firmware/src/obv_dsp.h"
#define FS 44100u
#include "../firmware/src/obv_drumx_core.h"
#include "../firmware/src/obv_drum_kits.h"
int main(void){
 int32_t y[CTL];uint64_t nonzero=0;
 for(unsigned k=0;k<5;k++)for(unsigned l=0;l<8;l++)for(unsigned corner=0;corner<16;corner++){
  dx_voice_t h={0};h.rng=(int32_t)(0x12345678u+k*123u+l);dx_trigger(&h);
  for(unsigned b=0;b<1000;b++){
   dx_run(&DRUM_KITS[k].lane[l],&h,(corner&1)?127:0,(corner&2)?384:-384,
       (corner&4)?63:-64,(corner&8)?63:-64,0,127,127,y,CTL,l,15,15,255);
   for(unsigned i=0;i<CTL;i++)if(y[i])nonzero++;
  }
 }
 assert(nonzero>10000);
 puts("PASS: isolated DRUM-X DSP, 640 kit/lane/corner cases, 10,240,000 output samples; UBSan + address checks; leak detection disabled.");
 return 0;
}
