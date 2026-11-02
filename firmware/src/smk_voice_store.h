/* SPDX-License-Identifier: GPL-3.0-only */
/* VD1 private data bank, outside every declared boot file. Stable Obervave
 * sectors are read only. No audio or timer ISR is running during first clone.
 * Hooks operate on raw SPI bytes (never SFC-decrypted XIP addresses). */
#ifndef OBV_VOICE_STORE_H
#define OBV_VOICE_STORE_H
#define VOICE_DATA_LO 0x00057000u
#define VOICE_DATA_HI 0x0009D000u
#define VOICE_SETTINGS_AT 0x00098000u
#define VOICE_PSECT_AT 0x00099000u
#define VOICE_PFAV_AT 0x0009B000u
#define VOICE_OWNER_AT 0x00057000u
#define VOICE_SLOTS_AT 0x00058000u
#define VOICE_STABLE_SETTINGS_AT 0x0009D000u
#define VOICE_STABLE_PSECT_AT 0x000A6000u
#define VOICE_STABLE_PFAV_AT 0x000A8000u
#define VOICE_STABLE_OWNER_AT 0x000AF000u
#define VOICE_STABLE_SLOTS_AT 0x000C2000u
#define VOICE_APP_SLOT (320u*1024u)
#define VOICE_DATA_WRITE_OK(off,n) (FL_IN(off,n,0x55000u,0x57000u)||FL_IN(off,n,VOICE_SETTINGS_AT,VOICE_SETTINGS_AT+0x1000u)|| \
 FL_IN(off,n,VOICE_PSECT_AT,VOICE_PSECT_AT+0x2000u)||FL_IN(off,n,VOICE_PFAV_AT,VOICE_PFAV_AT+0x2000u)|| \
 FL_IN(off,n,VOICE_OWNER_AT,VOICE_OWNER_AT+0x1000u)||FL_IN(off,n,VOICE_SLOTS_AT,VOICE_SLOTS_AT+VOICE_PATTERN_COUNT*4u*4096u))
_Static_assert(0x4000u+0x120u+VOICE_APP_SLOT<VOICE_DATA_LO,"private bank overlaps app file");
#define VOICE_PATTERN_COUNT 16u
#define VOICE_LEGACY_COUNT 8u
#define VOICE_LAYOUT_VERSION 1u
_Static_assert(VOICE_SLOTS_AT+VOICE_PATTERN_COUNT*4u*0x1000u==VOICE_SETTINGS_AT,"private slots overlap metadata");
_Static_assert(VOICE_SETTINGS_AT+0x1000u==VOICE_PSECT_AT&&VOICE_PSECT_AT+0x2000u==VOICE_PFAV_AT&&VOICE_PFAV_AT+0x2000u==VOICE_DATA_HI,"metadata moved");
#ifdef OBV_VOICE_STORE_IMPLEMENTATION
static uint8_t voice_copy_sector[4096] __attribute__((section(".pool")));
static int voice_copy_one(uint32_t src,uint32_t dst,uint32_t copy){
    uint8_t verify[128];
    if(copy&&voice_store_read(src,voice_copy_sector,4096))return -1;
    if(voice_store_erase(dst))return -2;
    if(!copy)return 0;
    if(voice_store_write(dst,voice_copy_sector,4096))return -3;
    for(unsigned o=0;o<4096;o+=sizeof verify){
        voice_store_feed();
        if(voice_store_read(dst+o,verify,sizeof verify)||memcmp(verify,voice_copy_sector+o,sizeof verify))return -4;
    }
    return 0;
}
/* Return 0 already initialized, 1 cloned/empty initialized, negative on failure.
 * The owner tag is committed last; an interrupted first clone is retried. */
static int voice_store_prepare(const uint8_t want[16]){
    uint8_t tag[16];
    if(voice_store_read(VOICE_OWNER_AT,tag,16))return -1;
    if(!memcmp(tag,want,16))return 0;
    /* Unknown owner/layout: never silently erase. A partially committed first
     * initialization may contain only expected bytes or erased bytes. */
    for(unsigned j=0;j<16;j++)if(tag[j]!=0xFFu&&tag[j]!=want[j])return -8;
    if(voice_store_read(VOICE_STABLE_OWNER_AT,tag,16))return -1;
    uint32_t copy=!memcmp(tag,"SMK37DATAOBERVAV",16);
    if(voice_store_erase(VOICE_OWNER_AT))return -2;
    if(voice_copy_one(VOICE_STABLE_SETTINGS_AT,VOICE_SETTINGS_AT,copy))return -3;
    for(unsigned i=0;i<2;i++){
        voice_store_feed();
        if(voice_copy_one(VOICE_STABLE_PSECT_AT+i*4096u,VOICE_PSECT_AT+i*4096u,copy))return -4;
        if(voice_copy_one(VOICE_STABLE_PFAV_AT+i*4096u,VOICE_PFAV_AT+i*4096u,copy))return -5;
    }
    for(unsigned i=0;i<VOICE_PATTERN_COUNT*4u;i++){
        voice_store_feed();
        if(voice_copy_one(VOICE_STABLE_SLOTS_AT+i*4096u,VOICE_SLOTS_AT+i*4096u,copy&&i<VOICE_LEGACY_COUNT*4u))return -6;
    }
    if(voice_store_write(VOICE_OWNER_AT,want,16)||voice_store_read(VOICE_OWNER_AT,tag,16)||memcmp(tag,want,16))return -7;
    return 1;
}
#endif
#endif
