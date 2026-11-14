/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(void){engine_init();pf_init();pat_init(&pat);mm_boot();fm1_ms=1000;fm1_in.frames=400;fm1_in.wheel[0]=535;fm1_in.wheel[1]=620;
 for(unsigned t=0;t<4;t++)trk_load(t,t);static patch_t before[4];memcpy(before,live_p,sizeof before);
 smk_wheels();assert(!ctl_vib&&!ctl_wheel_bend);
 for(unsigned n=0;n<20;n++){
  mm_enter();assert(mm_on&&!ctl_vib&&!ctl_bend&&!ctl_wheel_bend);
  /* Incoming controllers while MIDI cannot edit a patch or update synth state. */
  mq[mq_w%64][0]=0xB0;mq[mq_w%64][1]=1;mq[mq_w++%64][2]=127;mm_discard_midi();
  ctl_vib=127;ctl_bend=8191;ctl_wheel_bend=8191;for(unsigned p=0;p<NPART;p++)pt[p].bend_sm=123456;
  mm_leave();assert(!mm_on&&!ctl_vib&&!ctl_bend&&!ctl_wheel_bend);
  smk_wheels();assert(!ctl_vib&&!ctl_wheel_bend);for(unsigned p=0;p<NPART;p++)assert(!pt[p].bend_sm);
  assert(!memcmp(before,live_p,sizeof before));
 }
 fm1_in.wheel[1]=485;smk_wheels();assert(ctl_vib==127);mm_enter();fm1_in.wheel[1]=620;mm_leave();smk_wheels();assert(!ctl_vib);
 mm_enter();fm1_in.wheel[1]=485;mm_leave();smk_wheels();assert(ctl_vib==127);fm1_in.wheel[1]=620;smk_wheels();assert(!ctl_vib);
 assert(!memcmp(before,live_p,sizeof before));puts("PASS: 20 synth/MIDI round trips preserve all four patches, clear residual modulation/bend/slew and reapply actual wheels even when cached readings match; raised and released mod wheel works after return.");
}
