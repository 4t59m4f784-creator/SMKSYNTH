/* SPDX-License-Identifier: GPL-3.0-only */
/* B1 read-only saved-bank export. No erase/program hooks exist here. */
#define B1_SIZE (0x9d000u-0x57000u)
static uint32_t b1_active,b1_deadline;
static int b1_can_begin(void);
static uint32_t b1_crc(const uint8_t*p,unsigned n){uint32_t c=~0u;while(n--){c^=*p++;for(unsigned k=0;k<8;k++)c=c&1u?(c>>1)^0xedb88320u:c>>1;}return ~c;}
static unsigned b1_pack(const uint8_t*p,unsigned n,uint8_t*out){uint32_t a=0;unsigned bits=0,o=0;while(n--){a|=(uint32_t)*p++<<bits;bits+=8;while(bits>=7){out[o++]=(uint8_t)(a&127);a>>=7;bits-=7;}}if(bits)out[o++]=(uint8_t)a;return o;}
#include "smk_restore.h"
static unsigned b2_unpack(const uint8_t*p,unsigned n,uint8_t*out){uint32_t a=0;unsigned bits=0,o=0;while(n--){a|=(uint32_t)*p++<<bits;bits+=7;if(bits>=8){out[o++]=(uint8_t)a;a>>=8;bits-=8;}}return o;}
static int b1_route(void){
 const uint8_t *p;uint32_t n;
 if(!b2_latched&&b1_active&&(int32_t)(ota_now_ms()-b1_deadline)>=0)b1_active=0;
 if(!ota_frame_get(&p,&n)||n<5||memcmp(p,"\x7dSMKB",5))return 0;
 uint8_t req[15],data[513],wire[620],payload[512];unsigned len=0,status=0;
 if(n<15){ota_frame_done();return 1;}memcpy(req,p,15);
 unsigned extra=n-15,payload_n=0;uint32_t payload_crc=0;
 if(extra){unsigned raw_n=req[12]|(unsigned)req[13]<<7,enc=(raw_n*8+6)/7;
  if(raw_n<=512&&extra==enc+5){payload_n=b2_unpack(p+15,enc,payload);uint8_t c[4];b2_unpack(p+15+enc,5,c);payload_crc=b2_le(c);}else payload_n=513;
 }ota_frame_done();
 unsigned check=0;for(unsigned i=0;i<14;i++)check^=req[i];
 uint32_t off=req[9]|(uint32_t)req[10]<<7|(uint32_t)req[11]<<14;
 unsigned count=req[12]|(unsigned)req[13]<<7,cmd=req[6];
 if(req[5]!=1||check!=req[14]||(extra&&(payload_n>512||b1_crc(payload,payload_n)!=payload_crc)))status=1;
 else if(cmd==1){memcpy(data,"SMK16P1",7);data[7]=(uint8_t)B1_SIZE;data[8]=(uint8_t)(B1_SIZE>>8);data[9]=(uint8_t)(B1_SIZE>>16);data[10]=(uint8_t)(B1_SIZE>>24);len=11;}
 else if(cmd==2){if(b2_latched||!b1_can_begin())status=2;else b1_active=1;}
 else if(cmd==3){if(!b1_active)status=3;else if(!count||count>128||off>B1_SIZE||count>B1_SIZE-off)status=4;else if(st_read(0x57000u+off,data,count))status=5;else len=count;}
 else if(cmd==4){if(!b2_latched)b1_active=0;}
 else if(cmd==5){memcpy(data,"B2R1",4);len=4;}
 else if(cmd==6){
  if(!b1_active)status=3;else if(!count||count>512||off>B1_SIZE||count>B1_SIZE-off)status=4;
  else {uint8_t raw[512];if(st_read(0x57000u+off,raw,count))status=5;else{
   unsigned z=1;data[0]=1;for(unsigned i=0;i<count;){unsigned k=1;while(i+k<count&&k<255&&raw[i+k]==raw[i])k++;if(z+2>=count+1){z=count+1;break;}data[z++]=(uint8_t)k;data[z++]=raw[i];i+=k;}
   if(z>=count+1){data[0]=0;memcpy(data+1,raw,count);z=count+1;}len=z;
  }}
 }
 else if(cmd==7){uint32_t c;if(!b1_active)status=3;else if(!count||count>4096||off>B1_SIZE||count>B1_SIZE-off)status=4;else if(b2_flash_crc(0x57000u+off,count,&c))status=5;else{b2_put(data,c);len=4;}}
 else if(cmd==8){if(payload_n!=36||off)status=4;else if(!b2_latched&&(b1_active||!b1_can_begin()))status=2;else {status=b2_begin(payload);if(b2_latched)b1_active=1;}}
 else if(cmd==9){status=b2_upload(off,payload,payload_n);}
 else if(cmd==10){status=payload_n==4?b2_commit_sector(off,b2_le(payload)):4;}
 else if(cmd==11){status=b2_latched?b2_finish():3;}
 else if(cmd==12){data[0]=b2_pending;data[1]=b2_valid;data[2]=b2_complete&&b2_latched;memcpy(data+3,b2_header+8,32);memcpy(data+35,b2_marks,B2_SECTORS);len=35+B2_SECTORS;}
 else if(cmd==13){if(!b2_latched||!b2_complete)status=3;else b2_reboot_at=ota_now_ms()+250u;}
 else status=1;
 if(b1_active)b1_deadline=ota_now_ms()+5000u;
 wire[0]=0xf0;memcpy(wire+1,req,9);wire[7]=(uint8_t)(cmd|0x40u);wire[10]=(uint8_t)status;
 wire[11]=req[9];wire[12]=req[10];wire[13]=req[11];wire[14]=(uint8_t)(len&127);wire[15]=(uint8_t)(len>>7);
 unsigned w=16+b1_pack(data,len,wire+16);uint32_t c=b1_crc(data,len);uint8_t crc[4]={(uint8_t)c,(uint8_t)(c>>8),(uint8_t)(c>>16),(uint8_t)(c>>24)};
 w+=b1_pack(crc,4,wire+w);wire[w++]=0xf7;ota_wire_send(wire,w);return 1;
}
