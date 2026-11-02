/* SPDX-License-Identifier: GPL-3.0-only */
/* B2 maintenance journal. Outside boot CRCs, private data, and legacy banks.
 * A restore is resumable, not an atomic replacement of the entire bank. */
#define B2_JOURNAL 0x55000u
#define B2_STAGE 0x56000u
#define B2_SECTORS 70u
static uint8_t b2_pending,b2_valid,b2_complete;
static volatile uint8_t b2_latched;
static uint32_t b2_stage_sector=~0u,b2_received,b2_reboot_at;
static uint8_t b2_header[48],b2_marks[B2_SECTORS];
static int b2_read(uint32_t off,void*p,unsigned n){return st_read(off,p,n);}
static int b2_write(uint32_t off,const void*p,unsigned n){return fl_write(off,p,n);}
static int b2_erase(uint32_t off){uint32_t took;return fl_erase4k(off,&took);}
static uint32_t b2_le(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void b2_put(uint8_t*p,uint32_t n){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(n>>(8*i));}
static int b2_flash_crc(uint32_t off,unsigned n,uint32_t*out){uint8_t a[128];uint32_t c=~0u;while(n){unsigned k=n>sizeof a?sizeof a:n;if(b2_read(off,a,k))return -1;for(unsigned i=0;i<k;i++){c^=a[i];for(unsigned j=0;j<8;j++)c=c&1u?(c>>1)^0xedb88320u:c>>1;}off+=k;n-=k;fm1_wdt_feed();}*out=~c;return 0;}
static void b2_scan(void){
 uint8_t state=255;unsigned blank=1;b2_valid=b2_pending=b2_complete=0;
 if(b2_read(B2_JOURNAL,b2_header,48)||b2_read(B2_JOURNAL+64,&state,1)||b2_read(B2_JOURNAL+128,b2_marks,B2_SECTORS)){b2_pending=1;return;}
 for(unsigned i=0;i<48;i++)blank&=b2_header[i]==255;
 if(blank&&state==255){for(unsigned i=0;i<B2_SECTORS;i++)blank&=b2_marks[i]==255;if(blank)return;}
 b2_valid=!memcmp(b2_header,"SMKRST2!",8)&&b1_crc(b2_header+8,36)==b2_le(b2_header+44);
 unsigned all_done=1;for(unsigned i=0;i<B2_SECTORS;i++)all_done&=b2_marks[i]==0;
 b2_complete=b2_valid&&state==0&&all_done;
 b2_pending=!b2_complete;
}
static int b2_begin(const uint8_t*p){
 if(b2_valid&&b2_pending){if(memcmp(b2_header+8,p,36))return 8;b2_latched=1;return 0;}
 /* Commit metadata before touching a target. A torn journal blocks normal boot. */
 b2_pending=b2_latched=1;b2_complete=b2_valid=0;b2_stage_sector=~0u;b2_received=0;
 if(b2_erase(B2_JOURNAL))return 5;
 memcpy(b2_header,"SMKRST2!",8);memcpy(b2_header+8,p,36);b2_put(b2_header+44,b1_crc(p,36));
 if(b2_write(B2_JOURNAL+8,b2_header+8,40)||b2_write(B2_JOURNAL,b2_header,8))return 5;
 b2_scan();return b2_valid?0:5;
}
static int b2_upload(uint32_t off,const uint8_t*p,unsigned n){
 if(!b2_pending||!b2_valid)return 3;
 if(n!=512||off>=B1_SIZE||off%512)return 4;
 unsigned sector=off/4096,pos=off%4096;
 if(!pos){b2_stage_sector=~0u;b2_received=0;if(b2_erase(B2_STAGE))return 5;b2_stage_sector=sector;}
 if(b2_stage_sector!=sector)return 9;
 if(pos<b2_received){uint8_t v[512];return pos+512==b2_received&&!b2_read(B2_STAGE+pos,v,512)&&!memcmp(v,p,512)?0:9;}
 if(pos!=b2_received)return 9;
 if(b2_write(B2_STAGE+pos,p,n))return 5;b2_received+=n;return 0;
}
static int b2_commit_sector(uint32_t off,uint32_t want){
 if(!b2_pending||!b2_valid)return 3;
 if(off>=B1_SIZE||off%4096)return 4;
 unsigned sector=off/4096;uint32_t got;
 if(!b2_flash_crc(0x57000u+off,4096,&got)&&got==want){uint8_t zero=0;if(b2_marks[sector]&&b2_write(B2_JOURNAL+128+sector,&zero,1))return 5;b2_marks[sector]=0;return 0;}
 if(b2_stage_sector!=sector||b2_received!=4096)return 9;
 if(b2_flash_crc(B2_STAGE,4096,&got)||got!=want)return 6;
 if(b2_erase(0x57000u+off))return 5;
 uint8_t v[128];for(unsigned n=0;n<4096;n+=sizeof v){fm1_wdt_feed();if(b2_read(B2_STAGE+n,v,sizeof v)||b2_write(0x57000u+off+n,v,sizeof v))return 5;}
 if(b2_flash_crc(0x57000u+off,4096,&got)||got!=want)return 6;
 uint8_t zero=0;if(b2_write(B2_JOURNAL+128+sector,&zero,1))return 5;b2_marks[sector]=0;return 0;
}
static int b2_finish(void){
 uint32_t got;if(!b2_valid)return 3;
 for(unsigned i=0;i<B2_SECTORS;i++)if(b2_marks[i])return 9;
 if(b2_flash_crc(0x57000u,B1_SIZE,&got)||got!=b2_le(b2_header+40))return 6;
 uint8_t owner[16];if(b2_read(0x57000u,owner,16)||memcmp(owner,"SMK37DATASMK16P1",16))return 6;
 uint8_t zero=0;if(b2_write(B2_JOURNAL+64,&zero,1))return 5;
 b2_complete=1;/* Stay latched until reboot: no old RAM state may save over restored data. */return 0;
}
