/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
static uint8_t flash[0x100000],target[0x46000],rx[640],tx[640];
static uint32_t rx_n,tx_n,now,ops,cut,ready=1;
static void fm1_wdt_feed(void){}
static uint32_t ota_now_ms(void){return now;}
static int ota_frame_get(const uint8_t**p,uint32_t*n){*p=rx;*n=rx_n;return rx_n!=0;}
static void ota_frame_done(void){rx_n=0;}
static int ota_wire_send(const uint8_t*p,uint32_t n){assert(n<=sizeof tx);memcpy(tx,p,n);tx_n=n;return 0;}
static int st_read(uint32_t o,void*p,uint32_t n){assert(o>=0x55000&&o+n<=0x9d000);memcpy(p,flash+o,n);return 0;}
static int fl_write(uint32_t o,const uint8_t*p,uint32_t n){assert(o>=0x55000&&o+n<=0x9d000);unsigned partial=++ops==cut;for(unsigned i=0;i<(partial?n/2:n);i++)flash[o+i]&=p[i];return partial?-1:0;}
static int fl_erase4k(uint32_t o,uint32_t*t){(void)t;assert(o>=0x55000&&o+4096<=0x9d000&&!(o&4095));unsigned partial=++ops==cut;memset(flash+o,255,partial?2048:4096);return partial?-1:0;}
static int b1_can_begin(void){return ready;}
#include "../firmware/src/smk_backup.h"
static void reset(void){b2_pending=b2_valid=b2_complete=b2_latched=b1_active=0;b2_stage_sector=~0u;b2_received=0;b2_scan();if(b2_pending)b2_latched=b1_active=1;}
static void initial(void){memset(flash,0xA5,sizeof flash);memset(flash+0x55000,255,0x48000);memcpy(flash+0x57000,"SMK37DATASMK16P1",16);ops=cut=0;reset();assert(!b2_pending);}
static uint8_t meta[36];
static int restore(void){int rc=b2_begin(meta);if(rc)return rc;b2_latched=b1_active=1;for(unsigned s=0;s<70;s++){uint32_t off=s*4096,c=b1_crc(target+off,4096);rc=b2_commit_sector(off,c);if(rc==9){for(unsigned p=0;p<4096;p+=512){rc=b2_upload(off+p,target+off+p,512);if(rc)return rc;}rc=b2_commit_sector(off,c);}if(rc)return rc;}return b2_finish();}
static unsigned ask(unsigned cmd,unsigned off,unsigned count,const uint8_t*payload){memcpy(rx,"\x7dSMKB",5);rx[5]=1;rx[6]=cmd;rx[7]=1;rx[8]=0;for(unsigned i=0;i<3;i++)rx[9+i]=off>>(7*i)&127;rx[12]=count&127;rx[13]=count>>7;rx[14]=0;for(unsigned i=0;i<14;i++)rx[14]^=rx[i];rx_n=15;if(payload){rx_n+=b1_pack(payload,count,rx+rx_n);uint8_t c[4];b2_put(c,b1_crc(payload,count));rx_n+=b1_pack(c,4,rx+rx_n);}assert(rx_n<=640);assert(b1_route());assert(tx_n<=640);return tx[10];}
int main(int argc,char**argv){assert(argc==2);FILE*f=fopen(argv[1],"rb");assert(f&&fread(target,1,sizeof target,f)==sizeof target);fclose(f);for(unsigned i=0;i<32;i++)meta[i]=(uint8_t)(i+1);b2_put(meta+32,b1_crc(target,sizeof target));
 initial();assert(!restore()&&b2_complete&&b2_latched&&!memcmp(target,flash+0x57000,sizeof target));unsigned total=ops;reset();assert(b2_complete&&!b2_pending&&!b2_latched);
 unsigned cases=0;for(unsigned n=1;n<=64;n++){initial();cut=n;int rc=restore();assert(rc);reset();cut=0;assert(!restore());assert(!memcmp(target,flash+0x57000,sizeof target));assert(flash[0x54fff]==0xa5&&flash[0x9d000]==0xa5);cases++;}
 unsigned later[]={total/2,total-2,total-1,total};for(unsigned i=0;i<4;i++){initial();cut=later[i];assert(restore());reset();cut=0;assert(!restore()&&!memcmp(target,flash+0x57000,sizeof target));cases++;}
 initial();assert(!ask(5,0,0,0));assert(!ask(8,0,36,meta));assert(b2_pending&&b2_latched);uint8_t wrong[36];memcpy(wrong,meta,36);wrong[0]^=1;assert(ask(8,0,36,wrong)==8);
 assert(ask(9,sizeof target,512,target)==4);assert(ask(9,1,512,target)==4);assert(!ask(9,0,512,target));assert(tx_n<32);assert(ask(11,0,0,0)==9);assert(!ask(4,0,0,0)&&b1_active);now=6000;assert(!ask(12,0,0,0)&&b1_active);
 assert(!restore());assert(!ask(6,0,512,0));FILE*w=fopen("build/b2-wire.bin","wb");assert(w);fwrite(tx,1,tx_n,w);fclose(w);assert(!ask(7,0,4096,0));assert(!ask(13,0,0,0)&&b2_reboot_at);
 /* Incompressible largest reply exercises streaming beyond the TX ring. */
 for(unsigned i=0;i<512;i++)flash[0x57000+i]=(uint8_t)i;assert(!ask(6,0,512,0));assert(tx_n>192&&tx_n<620);
 printf("PASS: %u torn erase/program interruption points, journal recovery, exact B1 bank restoration, protected bytes, hash binding, payload framing, RLE/raw max reply, timeout latch and reboot. %u full-restore operations.\n",cases,total);
}
