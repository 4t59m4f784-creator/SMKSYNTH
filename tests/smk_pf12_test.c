/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static unsigned checks;
#define C(x) do{checks++;assert(x);}while(0)
static void faders(void){smk_faders(&dirty_page,&dirty_hdr,&dirty_tags);}
int main(void){
 engine_init();memset(&set,0,sizeof set);for(unsigned t=0;t<4;t++)trk_load(t,t);
 fx_sync();mix_apply();pf_init();fm1_in.frames=1000;fm1_ms=5000;smk_bank=1;
 patch_t effects=live_p[0];
 for(unsigned t=0;t<4;t++) {
  seq_trk=t;set.mix_cho[t]=60;set.mix_dly[t]=70;set.mix_rev[t]=80;
  for(unsigned i=0;i<4;i++){smk_fad_last[i]=255;fm1_in.fader[i]=10;}
  faders();for(unsigned i=0;i<3;i++)fm1_in.fader[i]=20;faders();
  C(set.mix_cho[t]==60&&set.mix_dly[t]==70&&set.mix_rev[t]==80);
  for(unsigned i=0;i<3;i++)fm1_in.fader[i]=90;faders();
  C(set.mix_cho[t]==90&&set.mix_dly[t]==90&&set.mix_rev[t]==90);
  for(unsigned i=0;i<3;i++)fm1_in.fader[i]=35;faders();
  C(set.mix_cho[t]==35&&set.mix_dly[t]==35&&set.mix_rev[t]==35);
  C(!memcmp(&effects,&live_p[0],sizeof effects));
  pf_snapshot(&pf_store.slot[t],t);pf_store.used|=1u<<t;
  set.mix_cho[t]=7;set.mix_dly[t]=8;set.mix_rev[t]=9;pf_recall(t,t);
  C(set.mix_cho[t]==35&&set.mix_dly[t]==35&&set.mix_rev[t]==35&&pf_matches(t,t));
  set.mix_cho[t]++;C(!pf_matches(t,t));
  set.mix_cho[t]=100;faders();fm1_in.fader[0]=40;faders();C(set.mix_cho[t]==100);
  fm1_in.fader[0]=105;faders();C(set.mix_cho[t]==105);
 }
 C(pf_valid(&pf_store));C(sizeof(pf_store_t)<=SETTINGS_MAX);
 pf_manual_write=1;pf_dirty=1;pf_t=0;ctl_seq_run=0;pf_save_poll();
 C(!pf_dirty);pf_init();C(pf_store.used==15&&pf_store.slot[2].cho==35);
 pf_store_v1_t old={0};old.tag=PF_TAG_V1;old.used=1;old.generation=10;
 old.slot[0].patch=live_p[0];old.slot[0].prog=0;old.slot[0].dly=0;old.slot[0].rev=63;old.slot[0].colour=0;
 memset(host_perf,0,sizeof host_perf);memset(host_perf_len,0,sizeof host_perf_len);
 C(!perf_store_save(0,&old,sizeof old));unsigned writes=host_write_count[3];
 pf_init();C(pf_store.used==1&&pf_store.generation==10&&pf_store.slot[0].cho==128);
 C(host_write_count[3]==writes&&!pf_dirty&&!pf_manual_write);
 set.mix_cho[1]=47;pf_recall(0,1);C(set.mix_cho[1]==47&&set.mix_dly[1]==0&&set.mix_rev[1]==63);
 pf_snapshot(&pf_store.slot[0],1);C(pf_store.slot[0].cho==47);
 pf_store.generation=11;C(!perf_store_save(1,&pf_store,sizeof pf_store));
 pf_init();C(pf_store.generation==11&&pf_store.slot[0].cho==47);
 ((pf_store_t*)host_perf[1])->tag=0;pf_init();C(pf_store.generation==10&&pf_store.slot[0].cho==128);
 printf("PF12: %u checks passed: selected sends, pickup/recall/track changes, shared FX isolation, chorus favourites, V1/V2 migration and fallback, no migration writes.\n",checks);
 return 0;
}
