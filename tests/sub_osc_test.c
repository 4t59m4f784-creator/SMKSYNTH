/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
#include <stdio.h>
static void render(unsigned blocks){int32_t out[2*CTL];for(unsigned b=0;b<blocks;b++){memset(out,0,sizeof out);voice_block(&vc[0],0);voice_render(&vc[0],out);}}
int main(void){
 _Static_assert(sizeof(patch_t)==80,"patch layout must not grow");
 _Static_assert(sizeof(pf_store_t)==708,"favorites unchanged");
 engine_init();pf_init();pat_init(&pat);set.tempo=90;set.m_vol=100;set.m_dj=64;
 for(unsigned t=0;t<4;t++)patch_from_factory(&live_p[t],&FACT_INIT);
 patch_t old=live_p[0];assert(!patch_sub_level(&old));patch_super_mix_set(&old,0,91);patch_super_mix_set(&old,1,39);
 for(unsigned wave=0;wave<2;wave++)for(unsigned oct=0;oct<2;oct++)for(unsigned level=0;level<=100;level++){
  patch_t p=old;patch_sub_set(&p,wave,oct,level);assert(patch_sub_wave(&p)==wave&&patch_sub_oct(&p)==oct&&patch_sub_level(&p)==level);
  assert(patch_super_mix(&p,0)==91&&patch_super_mix(&p,1)==39);assert(!memcmp(p.v,old.v,74));
  patch_super_mix_set(&p,0,12);assert(patch_sub_level(&p)==level&&patch_sub_oct(&p)==oct&&patch_sub_wave(&p)==wave);
 }
 patch_t drum;obv_drums_preset(&drum,1);patch_t copy=drum;patch_sub_set(&drum,1,1,100);assert(!memcmp(&copy,&drum,80));
 patch_t p=old;patch_sub_set(&p,1,1,73);prog_put(74,&p);prog_get(74,&copy);assert(!memcmp(&p,&copy,80));
 live_p[0]=p;snd_capture(0);psnd_t scene=psnd[0],back;pattern_t loaded;assert(!pattern_slot_save(4,&pat,&scene));assert(!pattern_slot_load(4,&loaded,&back));assert(!memcmp(&scene,&back,sizeof scene));assert(patch_sub_level(&back.p[0])==73);
 pf_snapshot(&pf_store.slot[0],0);pf_store.used|=1u;patch_defaults(&live_p[0]);pf_recall(0,0);assert(patch_sub_level(&live_p[0])==73&&patch_sub_wave(&live_p[0])==1);
 /* UI mappings are the actual OSC1/2 positions, not new encoder indices. */
 assert(PAGE[PG_OSC1].p[0]==P_O1_WAVE&&PAGE[PG_OSC1].p[2]==P_O1_LVL&&PAGE[PG_OSC2].p[3]==P_O2_SEMI);
 page=PG_OSC2;assert(super_edit_page()==PG_SUB);page=PG_SUB;assert(super_edit_page()==PG_OSCMOD);
 seq_trk=0;layer=L_NONE;memset(host_enc,0,sizeof host_enc);host_enc[0]=1;panel_encoders();assert(patch_sub_wave(&live_p[0])==1);
 host_enc[3]=-1;panel_encoders();assert(!patch_sub_oct(&live_p[0]));host_enc[2]=1;panel_encoders();assert(patch_sub_level(&live_p[0])==74);
 old=live_p[0];host_enc[1]=4;panel_encoders();assert(!memcmp(&old,&live_p[0],80));
 patch_from_factory(&live_p[0],&FACT_INIT);live_p[0].v[P_VINTAGE]=0;live_p[0].v[P_LFO_PITCH]=live_p[0].v[P_MOD_DEPTH]=0;
 for(unsigned wave=0;wave<2;wave++)for(unsigned oct=0;oct<2;oct++){
  patch_sub_set(&live_p[0],wave,oct,100);block_globals(&pt[0]);voice_start(&vc[0],0,69,127,0,0);render(64);
  assert(vc[0].sub_gain==pt[0].sub_level);uint32_t ph=vc[0].sub.ph;render(1);assert(vc[0].sub.ph-ph==(vc[0].sub_inc>>(oct+1))*CTL);
 }
 /* Unison card detune changes OSC1 but not the SUB fundamental. */
 block_globals(&pt[0]);voice_start(&vc[0],0,69,127,100,0);voice_block(&vc[0],0);assert(vc[0].sub_inc==pitch_inc(69*256)&&vc[0].inc1!=vc[0].sub_inc);
 for(unsigned n=0;n<200;n++){patch_sub_set(&live_p[0],n&1,(n>>1)&1,n%101);block_globals(&pt[0]);render(2);assert(vc[0].sub_gain>=0&&vc[0].sub_gain<=127*256);}
 patch_sub_set(&live_p[0],0,0,0);block_globals(&pt[0]);render(64);assert(vc[0].sub_gain==0);uint32_t ph=vc[0].sub.ph;render(1);assert(vc[0].sub.ph!=ph);
 page=PG_SUB;layer=L_NONE;seq_trk=0;
 for(unsigned theme=0;theme<2;theme++){zm_zoom=theme;draw_page();write_ppm_as(theme?"build/su1/sub-zoom.ppm":"build/su1/sub-obervave.ppm");}
 printf("PASS SUB: legacy defaults; packed extensions; SUPER coexistence; DRUM isolation; preset/pattern persistence; real K1/K3/K4; navigation; phase/frequency; unison fundamental; rapid edits; silent phase. voice=%zu part=%zu worker=%zu\n",sizeof(voice_t),sizeof(part_t),sizeof(dc_work));
}
