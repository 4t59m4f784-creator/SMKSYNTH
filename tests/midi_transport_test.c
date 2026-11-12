/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#define RING_PUBLISH() ((void)0)
static unsigned ready,count;static uint8_t wire[1024];
static unsigned smk_uart1_tx_done(void){return ready;}
static void smk_uart1_tx_put(unsigned b){wire[count++]=b;ready=0;}
#include "../firmware/src/smk_midi_transport.h"
int main(void){
 smk_tx_message(0x92,60,100);smk_tx_message(0x82,60,0);
 smk_tx_poll();assert(count==1&&wire[0]==0x92);for(unsigned i=0;i<1000;i++)smk_tx_poll();assert(count==1);
 for(unsigned i=0;i<5;i++){ready=1;smk_tx_poll();}assert(count==6&&wire[1]==60&&wire[2]==100&&wire[3]==0x82&&wire[4]==60&&!wire[5]);
 for(unsigned i=0;i<170;i++){assert(smk_tx_ready());smk_tx_message(0x90,64,100);}assert(!smk_tx_ready());
 while(smk_tx_r!=smk_tx_w){ready=1;smk_tx_poll();}assert(smk_tx_ready());
 puts("PASS: UART status/data order, byte-ready backpressure, no busy wait, queue capacity and wraparound; no hardware accessed.");
}
