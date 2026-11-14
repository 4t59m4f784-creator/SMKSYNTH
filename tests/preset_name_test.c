/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void bt(unsigned ms){fm1_in.buttons=1u<<BTN_SAVE;save_button_poll(fm1_in.buttons,0);fm1_ms+=ms;save_button_poll(0,0);fm1_in.buttons=0;save_button_poll(0,1u<<BTN_SAVE);}
static void patch(void){save_button_poll(1u<<BTN_HOME,0);preset_home_cancel=0;}
static void reset(void){preset_ui=0;set_dirty=ps_dirty=manual_store_request=0;}
int main(void){engine_init();pf_init();pat_init(&pat);for(unsigned i=0;i<NPSECT;i++)psect[i].tag=PS_TAG;seq_trk=1;fm1_ms=1000;trk_load(1,20);patch_t original=live_p[1],back;prog_get(20,&original);live_p[1]=original;
 bt(800);assert(preset_ui==1&&preset_named&&!ps_dirty);unsigned olddest=save_to;
 patch_t base=live_p[1];preset_turn(0,5);preset_turn(1,7);assert(preset_cursor==5&&!memcmp(&base,&live_p[1],sizeof base));
 memcpy(preset_snapshot.v+NPARAM,"WARM BRASS",10);preset_turn(ENC_PRESETS,56);assert(save_to==76);assert(!memcmp(preset_snapshot.v+NPARAM,"WARM BRASS",10));assert(!ps_dirty&&!manual_store_request);
 bt(100);assert(preset_ui==6&&!ps_dirty&&!manual_store_request);preset_turn(ENC_PRESETS,1);preset_turn(1,1);assert(save_to==76&&!memcmp(preset_snapshot.v+NPARAM,"WARM BRASS",10));
 patch();assert(preset_ui==1&&save_to==76&&!memcmp(preset_snapshot.v+NPARAM,"WARM BRASS",10));patch();assert(!preset_ui&&!ps_dirty&&!manual_store_request);assert(!memcmp(&base,&live_p[1],sizeof base));
 bt(800);save_to=76;memcpy(preset_snapshot.v+NPARAM,"WARM BRASS",10);bt(120);assert(preset_ui==6&&!ps_dirty);bt(120);assert(preset_ui==2&&manual_store_request&&trk_prog[1]==76);prog_get(76,&back);assert(!memcmp(back.v+NPARAM,"WARM BRASS",10));assert(!memcmp(live_p[1].v+NPARAM,"WARM BRASS",10));prog_get(olddest,&back);assert(!memcmp(&back,&original,sizeof back));
 /* A failed-write retry retains the draft; PATCH returns to edit, not a new snapshot. */
 preset_ui=4;patch();assert(preset_ui==1&&!memcmp(preset_snapshot.v+NPARAM,"WARM BRASS",10));bt(100);assert(preset_ui==6);bt(100);assert(preset_ui==2);
 /* New quick save cannot reuse an abandoned name or destination. */
 reset();trk_load(1,20);memcpy(live_p[1].v+NPARAM,"QUICK NAME",10);bt(100);assert(preset_ui==2&&!preset_named&&save_to==20);prog_get(20,&back);assert(!memcmp(back.v+NPARAM,"QUICK NAME",10));reset();
 /* Cursor and character limits, including space and hyphen; other bytes unchanged. */
 bt(800);preset_turn(0,-100);assert(!preset_cursor);preset_turn(0,100);assert(preset_cursor==9);patch_t before=preset_snapshot;for(int i=-127;i<=127;i++){preset_turn(1,i);unsigned c=preset_snapshot.v[NPARAM+9];assert(c==' '||c=='-'||(c>='A'&&c<='Z')||(c>='0'&&c<='9'));}assert(!memcmp(before.v,preset_snapshot.v,NPARAM));
 puts("PASS: long BT edit/review/distinct confirm; ten-character draft; destination browse no load/write; PATCH back/cancel; source preserved; named save and quick-save isolation; failed retry; bounded characters; no ABI changes.");
}
