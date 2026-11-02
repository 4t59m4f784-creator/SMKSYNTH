/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdint.h>
static unsigned hits[4][128];
static void grid_trace(int c,uint32_t t,uint32_t n){if(c=='S'&&t<4&&n<128)hits[t][n]++;}
#define TRACE(c,t,n) grid_trace(c,t,n)
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(unsigned n){while(n--){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}}
static void pad(unsigned p,unsigned v){uint32_t dirty=0;host_qput(0,(int32_t)(p<<8|v));smk_pads(48,&dirty);}
static unsigned total(unsigned t){unsigned n=0;for(unsigned i=0;i<128;i++)n+=hits[t][i];return n;}
int main(int argc,char **argv){
 engine_init();for(unsigned i=0;i<NPSECT;i++)psect[i].tag=PS_TAG;
 for(unsigned t=0;t<4;t++){trk_load(t,t*6);set.mix_lvl[t]=100;live_p[t].v[P_ARP_MODE]=A_OFF;}
 set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();pf_init();pat_init(&pat);layer=L_NONE;step_held=-1;
 obv_drums_preset(&live_p[0],0);render(3);
 /* Legacy size and scene offset: actual old-format slot, no automatic writeback. */
 pat.st[0][0].note[0]=36;pat.st[0][0].note[1]=61;pat.st[0][0].note[2]=72;
 pattern_t original=pat;snd_capture(1);psnd_t scene=psnd[1];
 assert(!slot_save(1,&pat,PAT_LEGACY_BYTES,&scene,sizeof scene));uint32_t writes=host_write_count[2];
 memset(&pat,0xCC,sizeof pat);pat_fetch(1,&pat);
 assert(!memcmp(&pat,&original,PAT_LEGACY_BYTES));assert(!memcmp(&psnd[1],&scene,sizeof scene));
 assert(pat.drum_magic==DRUM_SEQ_MAGIC&&!pat.drum_valid[0]);assert(drum_pattern_mask(&pat,0,0)==3);
 assert(host_write_count[2]==writes);
 /* Full mask is independent of the untouched melodic note array. */
 drum_pattern_set(&pat,0,0,255);assert(drum_pattern_mask(&pat,0,0)==255);
 assert(!memcmp(pat.st[0][0].note,original.st[0][0].note,SEQ_CHORD));
 patch_t kit=live_p[0];obv_analog_choose(0);assert(!memcmp(pat.st[0][0].note,original.st[0][0].note,SEQ_CHORD));live_p[0]=kit;render(3);
 pat.trk[0].len=1;ctl_seq_run=1;ctl_seq_restart=1;memset(hits,0,sizeof hits);render(1);
 for(unsigned l=0;l<8;l++)assert(hits[0][DRUM_NOTE[l]]==1);assert(total(0)==8);
 for(unsigned i=0;i<NV;i++)assert(!vc[i].gate);
 ctl_seq_run=0;render(1);
 /* The same mask path obeys chance, off, nudge and ratchet. */
 pat.st[0][0].chance=0;memset(hits,0,sizeof hits);ctl_seq_run=ctl_seq_restart=1;render(30);assert(!total(0));ctl_seq_run=0;render(1);
 pat.st[0][0].chance=100;pat.st[0][0].ratch=STEP_OFF;ctl_seq_run=ctl_seq_restart=1;render(30);assert(!total(0));ctl_seq_run=0;render(1);
 pat.st[0][0].ratch=2;pat.st[0][0].nudge=64;ctl_seq_run=ctl_seq_restart=1;render(8);assert(!total(0));render(12);assert(total(0)==24);ctl_seq_run=0;render(1);
 /* All 16 step pads edit one selected lane; no loss when several lanes share a step. */
 pat_init(&pat);page=PG_SEQ;pf_seq_mode=1;seq_trk=0;obv_context_refresh();assert(page==PG_DSEQ&&seq_view==SV_STEPS);
 for(unsigned l=0;l<8;l++){drum_ui_lane[0]=(uint8_t)l;pad(0,100);pad(0,0);}
 assert(drum_pattern_mask(&pat,0,0)==255);assert(!pat.st[0][0].note[0]);
 pad(0,100);pad(0,0);assert(drum_pattern_mask(&pat,0,0)==127);assert(seq_cur[0]==0);
 pat.trk[0].len=32;seq_pg[0]=1;drum_ui_lane[0]=5;pad(15,110);pad(15,0);assert(drum_pattern_mask(&pat,0,31)==32);
 seq_clear_cursor();assert(!drum_pattern_mask(&pat,0,31));pat=undo_pat;assert(drum_pattern_mask(&pat,0,31)==32);
 /* Copy/shift/length/undo move masks together with timing and automation container. */
 layer=L_EDIT;layer_used=0;edit_turn(0,1);assert(drum_pattern_mask(&pat,0,0)==32);pat=undo_pat;layer_used=0;edit_turn(1,1);assert(pat.trk[0].len==64&&drum_pattern_mask(&pat,0,63)==32);pat=undo_pat;layer=L_NONE;
 /* REC: top eight pads record all eight instruments in one step. */
 pat_init(&pat);seq_pg[0]=seq_cur[0]=0;page=PG_DSEQ;seq_view=SV_STEPS;rec=REC_READY;ctl_seq_run=0;fm1_ms=100;
 for(unsigned l=0;l<8;l++)pad(l,110);render(1);for(unsigned l=0;l<8;l++)pad(l,0);render(1);
 assert(drum_pattern_mask(&pat,0,0)==255);for(unsigned l=0;l<8;l++)assert(!seq_live(0,DRUM_NOTE[l]));
 rec=REC_OFF;ctl_seq_run=0;render(1);
 /* New format roundtrip includes masks and exact scene; old saved slot remains untouched. */
 snd_capture(2);scene=psnd[2];original=pat;assert(!slot_save(2,&pat,sizeof pat,&scene,sizeof scene));
 memset(&pat,0,sizeof pat);pat_fetch(2,&pat);assert(!memcmp(&pat,&original,sizeof pat));assert(!memcmp(&psnd[2],&scene,sizeof scene));
 slot_lens_read();assert((pat_used&6u)==6u);assert(slot_len[1][0]==16&&slot_len[2][0]==16);
 pattern_t legacy={0};assert(!slot_load(1,&legacy,PAT_LEGACY_BYTES,&scene,sizeof scene));assert(!legacy.drum_valid[0]);
 /* Saving an unedited legacy pattern retains the original layout. */
 pattern_t untouched=original;drum_pattern_init(&untouched);assert(!pattern_slot_save(3,&untouched,&scene));
 assert(!slot_load(3,&legacy,PAT_LEGACY_BYTES,&scene,sizeof scene));
 /* Boundary 256 and four-track queued swap: full masks travel on the bar. */
 drum_pattern_set(&pat,0,255,128);assert(drum_pattern_mask(&pat,0,255)==128);
 patch_from_factory(&live_p[2],&FACTORY[0]);obv_drums_preset(&live_p[3],2);render(3);
 pat_init(&pat);drum_pattern_set(&pat,0,0,85);drum_pattern_set(&pat,3,0,32);snd_capture(4);
 assert(!pattern_slot_save(4,&pat,&psnd[4]));slot_saved(4,&pat);
 drum_pattern_set(&pat,0,0,255);set.pat=2;ctl_seq_run=ctl_seq_restart=1;render(1);
 pat_go(4);assert(ctl_pat_swap==1);render(400);
 assert(!ctl_pat_swap&&drum_pattern_mask(&pat,0,0)==85&&drum_pattern_mask(&pat_old,0,0)==255);
 for(unsigned t=1;t<4;t++)assert(ui_seq_pos[t]==ui_seq_pos[0]);
 ctl_seq_run=0;render(1);
 /* Every mask byte, all four parts x 256 steps, survives storage. */
 pattern_t all=pat;for(unsigned t=0;t<4;t++)for(unsigned n=0;n<256;n++)drum_pattern_set(&all,t,n,(n+t*13u)&255u);
 assert(!pattern_slot_save(5,&all,&scene));assert(!pattern_slot_load(5,&pat_q,&scene));assert(!memcmp(&all,&pat_q,sizeof all));
 /* Track focus restores the original melodic sequencer and grid on returning. */
 trk_select(1);assert(page==PG_SEQ);trk_select(0);assert(page==PG_DSEQ&&seq_view==SV_STEPS);
 if(argc>1){ui_trk=0;draw_seq_top();ui_drum_grid(0,0,0,-1,drum_ui_lane[0],0);draw_seq_knobs();write_ppm_as(argv[1]);}
 printf("PASS: eight simultaneous trigger events, grid/REC/undo/shift/double, legacy + new slots and scenes, no automatic writes. pattern=%zu legacy=%u scene=%zu slot=%u extra=%zu bytes per RAM pattern.\n",sizeof pat,PAT_LEGACY_BYTES,sizeof(psnd_t),SLOT_MAX,sizeof pat-PAT_LEGACY_BYTES);
 return 0;
}
