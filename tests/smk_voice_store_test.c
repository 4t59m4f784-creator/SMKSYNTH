/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define BOARD_SMK37 1
#define OBV_VOICE_HW 1
#define OBV_DATA_FAMILY "SMK16P1"
#include "../firmware/src/smk_store.h"
static uint8_t flash[0x100000],original[0x100000];
static unsigned writes,erases,fail_at,mutations;
static int cut(void){return fail_at&&++mutations==fail_at;}
static int voice_store_read(uint32_t o,void*p,uint32_t n){assert(o+n<=sizeof flash);memcpy(p,flash+o,n);return 0;}
static int voice_store_write(uint32_t o,const void*p,uint32_t n){assert(FL_GUARD(o,n));if(cut())return -1;const uint8_t*x=p;for(unsigned i=0;i<n;i++){assert((flash[o+i]&x[i])==x[i]);flash[o+i]&=x[i];}writes++;return 0;}
static int voice_store_erase(uint32_t o){assert(!(o&4095)&&FL_GUARD(o,4096));if(cut())return -1;memset(flash+o,255,4096);erases++;return 0;}
static void voice_store_feed(void){}
#define OBV_VOICE_STORE_IMPLEMENTATION 1
#undef OBV_VOICE_STORE_H
#include "../firmware/src/smk_voice_store.h"
static void intact(void){
 assert(!memcmp(flash,original,VOICE_DATA_LO));
 assert(!memcmp(flash+VOICE_DATA_HI,original+VOICE_DATA_HI,sizeof flash-VOICE_DATA_HI));
}
static void cloned(void){
 assert(!memcmp(flash+VOICE_SETTINGS_AT,original+VOICE_STABLE_SETTINGS_AT,4096));
 assert(!memcmp(flash+VOICE_PSECT_AT,original+VOICE_STABLE_PSECT_AT,8192));
 assert(!memcmp(flash+VOICE_PFAV_AT,original+VOICE_STABLE_PFAV_AT,8192));
 assert(!memcmp(flash+VOICE_SLOTS_AT,original+VOICE_STABLE_SLOTS_AT,131072));for(unsigned i=VOICE_SLOTS_AT+131072;i<VOICE_SETTINGS_AT;i++)assert(flash[i]==255);intact();
}
int main(void){
 for(unsigned i=0;i<sizeof flash;i++)flash[i]=(uint8_t)(i*37u+(i>>12)*11u);
 memset(flash+VOICE_OWNER_AT,255,4096);memcpy(flash+VOICE_STABLE_OWNER_AT,"SMK37DATAOBERVAV",16);memcpy(original,flash,sizeof flash);
 uint8_t want[16];smk_owner_tag(want);assert(!memcmp(want,"SMK37DATASMK16P1",16));
 assert(voice_store_prepare(want)==1);cloned();unsigned m=writes+erases;
 assert(voice_store_prepare(want)==0&&m==writes+erases);intact();
 /* Cuts before each flash mutation, then retry from the unchanged stable source. */
 for(unsigned cut_at=1;cut_at<=m;cut_at++){
  memcpy(flash,original,sizeof flash);fail_at=cut_at;mutations=0;
  int rc=voice_store_prepare(want);intact();fail_at=0;
  if(rc<0){assert(voice_store_prepare(want)==1);cloned();}else assert(rc==1);
 }
 /* A foreign family is never imported or erased; only the private bank is initialized. */
 memcpy(flash,original,sizeof flash);memset(flash+VOICE_STABLE_OWNER_AT,0,16);memcpy(original,flash,sizeof flash);
 assert(voice_store_prepare(want)==1);intact();
 for(unsigned i=VOICE_SLOTS_AT;i<VOICE_DATA_HI;i++)assert(flash[i]==255);
 for(unsigned o=0;o<sizeof flash;o+=4096){
  int allowed=FL_GUARD(o,4096);int expected=VOICE_DATA_WRITE_OK(o,4096)||FL_IN(o,4096,FL_OTA_LO,FL_OTA_HI);assert(allowed==expected);
 }
 assert(!FL_GUARD(VOICE_DATA_LO-4096,8192)&&!FL_GUARD(VOICE_SETTINGS_AT-4096,8192)&&!FL_GUARD(VOICE_DATA_LO,0xFFFFF000));
 assert(!FL_GUARD(0x9D000,4096)&&!FL_GUARD(0xA6000,4096)&&!FL_GUARD(0xA8000,4096)&&!FL_GUARD(0xAF000,4096)&&!FL_GUARD(0xC2000,4096));
 memcpy(flash,original,sizeof flash);memcpy(flash+VOICE_OWNER_AT,"SMK37DATASMK16P2",16);unsigned before=writes+erases;assert(voice_store_prepare(want)==-8&&writes+erases==before);assert(!memcmp(flash+VOICE_OWNER_AT,"SMK37DATASMK16P2",16));
 printf("PASS: P16 private bank, raw clone/readback, stable data immutable, %u interrupted initializations/retry, foreign family, all-sector write allow-list and wrapping/straddling guards.\n",m);return 0;
}
