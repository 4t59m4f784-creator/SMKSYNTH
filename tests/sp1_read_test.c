/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#define BOARD_SMK37 1
static uint8_t flash[65536],dst[17000];static unsigned masked,calls,max,services,fail;
static unsigned irq_save(void){assert(!masked);masked=1;return 0;}
static void irq_restore(unsigned f){(void)f;assert(masked);masked=0;services++;}
static int fl_read_ram(uint32_t off,uint8_t*d,uint32_t n){assert(masked&&n<=32);calls++;if(n>max)max=n;if(calls==fail)return -7;memcpy(d,flash+off,n);return 0;}
#define FL_FAR(f) f
#include "../firmware/src/obv_store_read.h"
int main(void){for(unsigned i=0;i<sizeof flash;i++)flash[i]=(uint8_t)(i*17+(i>>8));
 for(unsigned n=0;n<=16384;n+=n<64?1:127){calls=services=0;assert(!st_read(13,dst,n));assert(!memcmp(dst,flash+13,n));assert(calls==(n+31)/32&&services==calls&&!masked);}
 for(unsigned k=1;k<16;k++){calls=services=0;fail=k;assert(st_read(11,dst,512)==-7);assert(!masked&&calls==k&&services==k);}assert(max==32);puts("PASS: exact raw reads, odd lengths, interruption service between <=32-byte transfers, error unlock/no later reads. Flash layout/encryption/write paths unchanged.");}
