/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void clear_battery(void){memset(&obv_battery,0,sizeof obv_battery);}
static void settle(uint32_t raw){for(uint32_t n=1;n<=80;n++)obv_battery_poll(n*1000u,raw);}
int main(void){
 _Static_assert(sizeof set==67,"saved globals must not grow");
 _Static_assert(sizeof(patch_t)==80,"saved patches must not grow");
 _Static_assert(sizeof(pf_store_t)==708,"saved favourites must not grow");
 const uint32_t raw[]={1,529,530,559,560,601,602,1022},level[]={1,1,2,2,3,3,4,4};
 for(unsigned i=0;i<8;i++){clear_battery();settle(raw[i]);assert(obv_battery.level==level[i]);}
 clear_battery();assert(!obv_battery_poll(999,700));assert(!obv_battery_poll(1000,700));assert(!obv_battery_poll(1999,700));assert(!obv_battery_poll(2000,700));assert(obv_battery_poll(3000,700)&&obv_battery.level==4);
 /* A lone bad raw value cannot turn a healthy battery red; no periodic redraw. */
 assert(!obv_battery_poll(4000,450)&&obv_battery.level==4);
 for(uint32_t ms=5000;ms<20000;ms+=1000)assert(!obv_battery_poll(ms,700));
 /* Missing/saturated readings clear the indication, then require settling again. */
 assert(obv_battery_poll(20000,0)&&!obv_battery.level);
 assert(!obv_battery_poll(21000,1023)&&!obv_battery.level);
 assert(!obv_battery_poll(22000,700));assert(!obv_battery_poll(23000,700));assert(obv_battery_poll(24000,700)&&obv_battery.level==4);
 clear_battery();obv_battery.ms=UINT32_MAX-499u;assert(!obv_battery_poll(499u,700));assert(!obv_battery_poll(500u,700));assert(obv_battery.count==1);
 engine_init();pf_init();pat_init(&pat);seq_trk=0;page=PG_PERFORM;
 unsigned writes[4];memcpy(writes,host_write_count,sizeof writes);
 uint8_t globals[sizeof set],patterns[sizeof pat],patches[sizeof live_p],favourites[sizeof pf_store];
 memcpy(globals,&set,sizeof set);memcpy(patterns,&pat,sizeof pat);memcpy(patches,live_p,sizeof live_p);memcpy(favourites,&pf_store,sizeof pf_store);
 static uint16_t reference[240*240];
 for(unsigned n=0;n<=4;n++){
  obv_battery.level=n;ui_header(70,(const uint8_t*)"LONG PATCH",1,"MONO NOTCH HOLD SEQ REC",3,8);
  if(!n)memcpy(reference,fb,sizeof reference);
  for(unsigned y=0;y<44;y++)for(unsigned x=0;x<240;x++)if(x<216||x>=236||y<25||y>=35)assert(fb[y*240+x]==reference[y*240+x]);
  assert(fb[25*240+216]==(n==1?C_RED:C_DIMT));
  char name[80];snprintf(name,sizeof name,"build/battery-%u.ppm",n);write_ppm_as(name);
 }
 assert(!memcmp(globals,&set,sizeof set)&&!memcmp(patterns,&pat,sizeof pat)&&!memcmp(patches,live_p,sizeof live_p)&&!memcmp(favourites,&pf_store,sizeof pf_store));
 assert(!memcmp(writes,host_write_count,sizeof writes));
 puts("PASS: stock band boundaries, 1s sampling, three-sample confirmation, transient filtering, invalid ADC, timer wrap, bounded header drawing and unchanged saved data/writes");
}
