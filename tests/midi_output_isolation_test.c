/* SPDX-License-Identifier: GPL-3.0-only */
/* Real USB ring + real TRS queue + production dispatcher; no fake midi_send. */
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#define BOARD_SMK37 1
#define FELUCCA_UAC 0
#define FELUCCA_CDC 0
#define FELUCCA_OTA 0
#define RING_PUBLISH() ((void)0)
static void fm1_delay_ms(uint32_t ms){(void)ms;}
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#include "../firmware/src/usb.c"
static uint8_t wire[8192];static unsigned count;
static unsigned smk_uart1_tx_done(void){return 1;}
static void smk_uart1_tx_put(unsigned b){assert(count<sizeof wire);wire[count++]=b;}
#include "../firmware/src/smk_midi_transport.h"
#include "../firmware/src/smk_midi_output.h"
static void cable_drain(void){while(smk_tx_r!=smk_tx_w)smk_tx_poll();}
int main(void){
 usb.config=1; /* Enumerated USB host never reads MIDI IN. */
 for(unsigned i=0;i<80;i++){
  assert(midi_send_ready());
  midi_send(0xb0|(i/5),120,0);cable_drain();
 }
 /* Entering MIDI MODE must not prevent the first single-channel note. */
 assert(midi_send_ready());midi_send(0x92,60,103);cable_drain();
 assert(count==243&&wire[240]==0x92&&wire[241]==60&&wire[242]==103);
 midi_send(0x82,60,0);cable_drain();
 assert(wire[243]==0x82&&wire[244]==60&&wire[245]==0);
 assert(mo_overflow&&mo_w-mo_r==MQ);
 uint32_t pkt;
 for(unsigned i=0;i<48;i++){
  assert(midi_out_take(&pkt));
  unsigned cc=i%3==0?64:i%3==1?123:120;
  assert(pkt==(0x0bu|(0xb0u|i/3)<<8|cc<<16));
  /* Playing while the USB reset drains must not interleave stale notes. */
  if(i<47){midi_send(0x93,62,99);cable_drain();}
 }
 assert(!midi_out_pending());
 /* Normal USB and TRS mirror the full, correct channel/status/data again. */
 count=0;
 for(unsigned ch=0;ch<16;ch++){
  const unsigned st[]={0x90,0xe0,0xb0,0xb0,0x80};
  const unsigned a[]={60,1,1,64,60},b[]={103,100,91,127,0};
  for(unsigned j=0;j<5;j++){
   assert(midi_send_ready());midi_send(st[j]|ch,a[j],b[j]);cable_drain();
   assert(midi_out_take(&pkt));
   assert(pkt==((st[j]>>4)|(st[j]|ch)<<8|a[j]<<16|b[j]<<24));
   unsigned k=(ch*5+j)*3;assert(wire[k]==(st[j]|ch)&&wire[k+1]==a[j]&&wire[k+2]==b[j]);
  }
 }
 assert(!midi_out_pending());
 /* Cable backpressure is still honored, with no overwrite. */
 for(unsigned i=0;i<170;i++){assert(midi_send_ready());midi_send(0x90,64,100);}
 assert(!midi_send_ready());unsigned w=smk_tx_w;midi_send(0x80,64,0);assert(smk_tx_w==w);cable_drain();assert(midi_send_ready());
 usb.config=0;mo_r=mo_w;mo_overflow=mo_recover=0;
 midi_send(0x97,67,110);cable_drain();assert(!midi_out_pending());
 puts("PASS: a USB host that does not consume MIDI cannot block TRS after mode-entry panic.");
 puts("PASS: USB overflow recovers with 48 safety messages; both streams retain channel/note/bend/mod/sustain; UART backpressure and standalone TRS preserved.");
}
