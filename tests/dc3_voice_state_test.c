/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(void){
 engine_init();for(unsigned p=0;p<NPART;p++)trk_load(p,0);
 live_p[0].v[P_O1_WAVE]=W_SUPER;block_globals(&pt[0]);
 /* Rendering can outlive the active envelope and old engine tag during fades.
  * The renderer's prepared waveform, not allocator eligibility, owns its state. */
 obv_patch_set_engine(&live_p[0],OBV_DRUMS);
 voice_start(&vc[0],0,60,100,0,0);voice_block(&vc[0],0);
 vc[0].ae.stage=E_IDLE;vc[0].gate=0;
 vc[1]=vc[0];
 super_state[0].active=0;super_state[1].active=0;
 voice_t before=vc[0];super_state_t ss=super_state[0];super_plain_state_t ps=super_plain_state[0];
 int32_t expected[2*CTL]={0};voice_render(&vc[0],expected);
 voice_t after=vc[0];super_state_t afterss=super_state[0];super_plain_state_t afterps=super_plain_state[0];
 vc[0]=before;super_state[0]=ss;super_plain_state[0]=ps;
 dc_enabled=1;uint32_t mask=dc_begin(3u);assert(mask==1u);assert(dc_super(0));
 static int32_t buses[NPART][2*HALF_FRAMES];dc_finish(mask,buses,0);
 assert(!memcmp(expected,buses[0],sizeof expected));assert(!memcmp(&vc[0],&after,sizeof after));
 assert(!memcmp(&super_state[0],&afterss,sizeof afterss));assert(!memcmp(&super_plain_state[0],&afterps,sizeof afterps));
 puts("PASS: idle-envelope and engine-transition SUPER waveform retains exact private render/commit states.");
}
