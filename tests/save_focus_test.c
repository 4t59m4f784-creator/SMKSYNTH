/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(unsigned n){while(n--){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}}
static unsigned gates(unsigned p,unsigned seq){unsigned n=0;for(unsigned i=0;i<NV;i++)n+=vc[i].gate&&vc[i].part==p&&vc[i].seq_owned==seq;return n;}
static void off(void){for(unsigned n=0;n<128;n++)note_in(0,n,0);ctl_sustain=0;ctl_seq_run=0;ctl_panic=1;render(2);memset(sr,0,sizeof sr);}
static void init_parts(void){for(unsigned t=0;t<4;t++){trk_load(t,0);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_GLIDE]=0;live_p[t].v[P_ARP_MODE]=A_OFF;live_p[t].v[P_VINTAGE]=0;live_p[t].v[P_LFO_AMP]=0;set.mix_lvl[t]=100;}}
static void combo(unsigned mode){keyboard_cfg_t k;keyboard_defaults(&k);k.mode=mode;keyboard_changed(k);}
static void pad(unsigned p,unsigned vel){uint32_t d=0;host_qput(0,(int32_t)(p<<8|vel));smk_pads(48,&d);}

static void tap_bt(unsigned ms){fm1_in.buttons=1u<<BTN_SAVE;save_button_poll(fm1_in.buttons,0);fm1_ms+=ms;save_button_poll(0,0);fm1_in.buttons=0;save_button_poll(0,1u<<BTN_SAVE);}
int main(void){engine_init();for(unsigned i=0;i<NPSECT;i++)psect[i].tag=PS_TAG;init_parts();set.m_vol=100;set.tempo=120;fx_sync();mix_apply();pf_init();pat_init(&pat);step_held=-1;layer=L_NONE;seq_trk=0;page=PG_SEQ;
 /* BT short is always preset-only, even on the sequencer and Split/Layer. */
 seq_trk=2;unsigned old=trk_prog[2];fm1_ms=100;tap_bt(120);assert(preset_ui==2&&preset_part==2&&save_to==old&&!pat_save_say&&!copy_to&&manual_store_request);save_ui_draw();write_ppm_as("build/u2-preset-writing.ppm");
 preset_ui=0;manual_store_request=0;page=PG_KEYBOARD;layer=L_SEQ;layer_lock=1;tap_bt(900);assert(preset_ui==1&&!layer&&!layer_lock&&!pat_save_say);save_to=42;save_ui_draw();write_ppm_as("build/u2-preset-as.ppm");tap_bt(120);assert(preset_ui==6);tap_bt(120);assert(preset_ui==2&&trk_prog[2]==42&&!pat_save_say);preset_ui=0;set_dirty=ps_dirty=manual_store_request=0;
 /* Existing current slot requires hold + distinct short tap, not hold release. */
 pat_used=1;set.pat=0;pf_manual_seq=2;pb_event(0,100);fm1_ms+=800;pb_save(0);assert(pb_confirm==1&&!pat_save_say&&!copy_to);pb_event(0,0);assert(pb_confirm==1&&!pat_save_say);save_ui_draw();write_ppm_as("build/u2-overwrite.ppm");
 assert(pb_led(0)!=pb_led(1));fm1_ms+=4000;pf_hold_poll();assert(!pb_confirm&&!pat_save_say&&!copy_to);
 pb_event(0,100);fm1_ms+=800;pb_save(0);pb_event(0,0);pb_event(0,100);fm1_ms+=100;pb_event(0,0);assert(!pb_confirm&&pat_save_say&&pattern_focus(&pat)==2);pat_save_say=pat_dirty=0;pb_write_target=0;
 /* Fresh slot does not ask overwrite; queued copy captures current focus. */
 pb_event(1,100);fm1_ms+=800;pb_save(1);assert(!pb_confirm&&copy_to==2&&pattern_focus(&pat)==2);copy_to=pb_write_target=0;
 /* Wrong pad and source pattern change cannot authorize overwrite. */
 pat_used|=4;pb_event(2,100);fm1_ms+=800;pb_save(2);pb_event(2,0);assert(pb_confirm==3);pb_event(3,100);assert(!pb_confirm&&!copy_to);pb_fired[3]=1;
 pb_event(2,100);fm1_ms+=800;pb_save(2);pb_event(2,0);set.pat=1;pf_hold_poll();assert(!pb_confirm&&!copy_to);set.pat=0;
 /* Focus stored without moving old bytes, scenes or keyboard layouts. */
 snd_capture(0);psnd_t scene=psnd[0],back;pattern_t loaded;pattern_focus_set(&pat,3);combo(KB_LAYER);pat.keyboard_selected=0;assert(!pattern_slot_save(4,&pat,&scene));assert(!pattern_slot_load(4,&loaded,&back)&&pattern_focus(&loaded)==3&&!memcmp(&scene,&back,sizeof scene));seq_trk=1;load_pattern(4);assert(seq_trk==3&&pat.keyboard.mode==KB_LAYER);assert(sizeof pat==14476&&sizeof scene==380);
 seq_trk=0;ctl_seq_run=ctl_seq_restart=1;render(1);pat_go(4);render(500);assert(!ctl_pat_swap&&seq_trk==3);off();
 /* Legacy files have no focus tag; they reliably select A. */
 assert(!slot_save(5,&pat,PAT_SL1_BYTES,&scene,sizeof scene));assert(!pattern_slot_load(5,&loaded,&back)&&pattern_focus(&loaded)==0);
 preset_ui=pb_confirm=0;page=PG_SEQ;seq_view=SV_SLOTS;lcd_fill(0,0,240,240,C_PANEL);draw_seq_top();draw_seq_slots();draw_seq_knobs();write_ppm_as("build/u2-pattern-page.ppm");
 puts("PASS: preset-only short/long BT across SEQ/keyboard/locked layer, hold+tap overwrite, release safety, 4s timeout/no write, empty copy, wrong-pad/source cancellation, exact-size focus roundtrip, stopped/queued focus recall, legacy A, scene bytes intact.");}
