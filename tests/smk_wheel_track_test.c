#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(void){
 engine_init();for(unsigned t=0;t<4;t++){trk_load(t,0);live_p[t].v[P_BEND]=2;}
 for(unsigned selected=0;selected<4;selected++){
  seq_trk=selected;smk_wheels();assert(ctl_wheel_part==selected);
  ctl_wheel_bend=4096;ctl_bend=0;
  for(unsigned i=0;i<200;i++)for(unsigned t=0;t<4;t++)block_globals(&pt[t]);
  for(unsigned t=0;t<4;t++)assert(pt[t].bend8==(t==selected?256:0));
 }
 ctl_wheel_bend=0;ctl_bend=-4096;
 for(unsigned i=0;i<200;i++)for(unsigned t=0;t<4;t++)block_globals(&pt[t]);
 for(unsigned t=0;t<4;t++)assert(pt[t].bend8==-256);
 ctl_bend=0;
 for(unsigned selected=0;selected<4;selected++){
  ctl_wheel_part=selected;
  for(unsigned t=0;t<4;t++){
   uint32_t inc[2];
   for(unsigned on=0;on<2;on++){
    pt[t].bend_sm=0;live_p[t].v[P_VINTAGE]=0;live_p[t].v[P_LFO_PHASE]=0;block_globals(&pt[t]);pt[t].lfo_val=32767;
    voice_start(&vc[0],t,60,100,0,0);vc[0].lfo_fade=32767;
    ctl_vib=on?127:0;voice_block(&vc[0],0);inc[on]=vc[0].inc1;
   }
   printf("selected%u part%u inc %u %u phase%d\n",selected,t,inc[0],inc[1],live_p[t].v[P_LFO_PHASE]);assert((inc[0]!=inc[1])==(t==selected));
  }
 }
 for(unsigned i=0;i<4;i++)assert(host_write_count[i]==0);
 puts("PASS: four-part wheel targeting, held-bend track switching, per-voice modulation isolation, MIDI bend retained, zero writes.");return 0;
}
