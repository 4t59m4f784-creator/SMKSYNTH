/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
static uint8_t request[640],reply[192];static uint32_t request_n,reply_n,now,ready=1,reads;
static uint32_t ota_now_ms(void){return now;}
static int ota_frame_get(const uint8_t**p,uint32_t*n){*p=request;*n=request_n;return request_n!=0;}
static void ota_frame_done(void){request_n=0;}
static int ota_wire_send(const uint8_t*p,uint32_t n){assert(n<=sizeof reply);memcpy(reply,p,n);reply_n=n;return 0;}
static int st_read(uint32_t off,void*p,uint32_t n){assert(off>=0x57000&&off+n<=0x9d000);reads++;for(unsigned i=0;i<n;i++)((uint8_t*)p)[i]=(uint8_t)(off+i);return 0;}
static int b1_can_begin(void){return ready;}
static void fm1_wdt_feed(void){}
static int fl_write(uint32_t o,const uint8_t*p,uint32_t n){(void)o;(void)p;(void)n;assert(0);return -1;}
static int fl_erase4k(uint32_t o,uint32_t*t){(void)o;(void)t;assert(0);return -1;}
#include "../firmware/src/smk_backup.h"
static void ask(unsigned cmd,unsigned off,unsigned n){memcpy(request,"\x7dSMKB",5);request[5]=1;request[6]=cmd;request[7]=3;request[8]=0;for(unsigned i=0;i<3;i++)request[9+i]=(uint8_t)(off>>(7*i)&127);request[12]=n&127;request[13]=n>>7;request[14]=0;for(unsigned i=0;i<14;i++)request[14]^=request[i];request_n=15;assert(b1_route());assert(reply[0]==240&&reply[reply_n-1]==247&&reply[7]==(cmd|64)&&reply[8]==3);}
int main(void){assert(b1_crc((const uint8_t*)"123456789",9)==0xcbf43926);ask(1,0,0);assert(!reply[10]);ready=0;ask(2,0,0);assert(reply[10]==2&&!b1_active);ready=1;ask(2,0,0);assert(!reply[10]&&b1_active);ask(3,0,128);assert(!reply[10]&&reply_n<=192);FILE*f=fopen("build/b1-wire.bin","wb");assert(f);fwrite(reply,1,reply_n,f);fclose(f);ask(3,B1_SIZE-128,128);assert(!reply[10]);unsigned before=reads;ask(3,B1_SIZE-1,128);assert(reply[10]==4&&reads==before);ask(3,0,129);assert(reply[10]==4);ask(3,0,0);assert(reply[10]==4);now=5001;ask(3,0,128);assert(reply[10]==3&&!b1_active);ask(2,0,0);ask(4,0,0);assert(!b1_active);memcpy(request,"OTHER",5);request_n=5;assert(!b1_route()&&request_n==5);puts("PASS: B1 protocol CRC, fixed namespace, bounds, BUSY, timeout, END, TX<192, no write hooks.");}
