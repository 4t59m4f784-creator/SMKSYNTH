/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#define FELUCCA_UAC 0
#define FELUCCA_CDC 0
#define FELUCCA_OTA 0
#define RING_PUBLISH() ((void)0)
static void fm1_delay_ms(uint32_t ms){(void)ms;}
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#include "../firmware/src/usb.c"
int main(void){
 assert(CFG_DESC[4]==2);unsigned as=0,midi=0,audio_ep=0;
 for(unsigned i=0;i<sizeof CFG_DESC;i+=CFG_DESC[i]){
  assert(CFG_DESC[i]>=2 && i+CFG_DESC[i]<=sizeof CFG_DESC);
  if(CFG_DESC[i+1]==4){as+=CFG_DESC[i+5]==1&&CFG_DESC[i+6]==2;midi+=CFG_DESC[i+5]==1&&CFG_DESC[i+6]==3;}
  if(CFG_DESC[i+1]==5)audio_ep+=CFG_DESC[i+2]==0x84;
 }
 assert(!as&&!audio_ep&&midi==1);
 usb.config=1;midi_out_event(0x00643c90);assert(mo_w==1);
 puts("PASS: actual MIDI-only descriptor: no audio-streaming interface/EP4; MIDI event ring operational. No physical USB test.");
}
