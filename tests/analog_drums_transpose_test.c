/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(unsigned n){while(n--){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}}
static unsigned gates(void){unsigned n=0;for(unsigned i=0;i<NV;i++)n+=!!vc[i].gate;return n;}
static void clean(void){ctl_seq_run=0;ctl_panic=1;render(2);memset(sr,0,sizeof sr);}
static void capture(unsigned n){note_in(1,n,100);note_in(0,n,0);}
int main(void){
 engine_init();for(unsigned i=0;i<NPSECT;i++)psect[i].tag=PS_TAG;
 for(unsigned t=0;t<4;t++){trk_load(t,0);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_ARP_MODE]=A_OFF;live_p[t].v[P_GLIDE]=0;live_p[t].v[P_VINTAGE]=0;set.mix_lvl[t]=100;}
 set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();pf_init();pat_init(&pat);step_held=-1;layer=L_NONE;
 assert(!obv_engine_available(1)&&obv_engine_available(0)&&obv_engine_available(2));
 /* Legacy engine tags retain ids. Only a live copy is replaced; no writes. */
 patch_t old=live_p[0];obv_patch_set_engine(&old,OBV_VOICE);old.v[OBV_ENGINE_TAG_AT+3]=2;
 old.v[P_CHO_DEPTH]=37;patch_t disk=old,loaded=old;assert(obv_patch_sanitize(&loaded));
 assert(obv_patch_engine(&loaded)==0&&loaded.v[P_CHO_DEPTH]==37);assert(!memcmp(&old,&disk,sizeof old));
 assert(!obv_patch_sanitize(&loaded));
 /* The same offset retunes four parts, without replacing note/gate/phase identity. */
 ctl_seq_transpose=0;for(unsigned t=0;t<4;t++){pat.st[t][0].note[0]=60+t;seq_sound(t,&pat.st[t][0],0);}render(2);
 assert(gates()==4);uint32_t base[NV],serial[NV];
 for(unsigned i=0;i<NV;i++){base[i]=vc[i].inc1;serial[i]=vc[i].seq;}
 pattern_t original=pat;ctl_seq_transpose=2;render(1);
 for(unsigned i=0;i<NV;i++)if(vc[i].gate){assert(vc[i].seq_owned);assert(ui_vnote[i]==vc[i].note+2);assert(vc[i].inc1>base[i]);assert(vc[i].seq==serial[i]);}
 assert(!memcmp(&pat,&original,sizeof pat));
 for(unsigned t=0;t<4;t++){seq_tie_into(t,&pat.st[t][0],0,1);assert(sr[t].snd[0]==60+t);seq_release_t(t);}assert(!gates());
 /* A live note in another part is never transposed. */
 clean();ctl_seq_transpose=0;voice_trigger(3,72,100);render(1);unsigned v=0;while(!vc[v].gate)v++;uint32_t inc=vc[v].inc1;
 ctl_seq_transpose=-1;render(1);assert(!vc[v].seq_owned&&ui_vnote[v]==72&&vc[v].inc1==inc);voice_release(3,72,0);assert(!gates());
 /* Captured notes do not sound, record, or edit steps. Key releases survive mode/context exit. */
 clean();page=PG_SEQ;seq_view=SV_TRANSPOSE;seq_trk=0;transpose_keys=1;rec=REC_READY;
 original=pat;capture(48);assert(ctl_seq_transpose==0);capture(50);assert(ctl_seq_transpose==2);capture(47);assert(ctl_seq_transpose==-1);
 assert(!gates()&&!memcmp(&pat,&original,sizeof pat));assert(!nheld[0]&&!rec_open[47].t);
 note_in(1,55,100);assert(transpose_captured[55]);page=PG_PERFORM;obv_context_refresh();assert(!transpose_keys&&ctl_seq_transpose==7);
 note_in(0,55,0);render(2);assert(!transpose_captured[55]&&!gates());rec=REC_OFF;
 /* Already-held live notes retain release ownership after focus/mode changes. */
 note_in(1,60,100);render(1);assert(gates());page=PG_SEQ;seq_view=SV_TRANSPOSE;transpose_keys=1;
 seq_trk=1;note_in(0,60,0);render(1);assert(!gates());
 /* Same key retrigger entering capture relinquishes the earlier live note. */
 transpose_keys=0;note_in(1,64,100);render(1);transpose_keys=1;note_in(1,64,100);render(1);assert(!gates());note_in(0,64,0);
 /* Capture OFF + same-key live retrigger must transfer note-off ownership. */
 capture(48);transpose_keys=1;note_in(1,65,100);transpose_keys=0;note_in(1,65,100);render(1);assert(gates());note_in(0,65,0);render(1);assert(!gates());transpose_keys=1;
 knob_view_t k={0};seq_view_knob(&k,0);assert(!strcmp(k.value,"ON"));seq_view_turn(0,-1);assert(!transpose_keys);
 seq_view_turn(1,1000);assert(ctl_seq_transpose==79);seq_view_turn(1,-1000);assert(ctl_seq_transpose==-48);
 seq_view_turn(2,1);assert(!ctl_seq_transpose);seq_view_turn(1,2);assert(ctl_seq_transpose==2);
 /* Manual pattern load, queued bar swap, transport stop and restart retain offset. */
 snd_capture(1);assert(!pattern_slot_save(1,&pat,&psnd[1]));pat_go(1);assert(ctl_seq_transpose==2);
 ctl_seq_run=ctl_seq_restart=1;render(1);pat_go(0);render(500);assert(!ctl_pat_swap&&ctl_seq_transpose==2);ctl_seq_run=0;render(2);assert(ctl_seq_transpose==2);
 /* All four tracks can be DRUM-X; global transpose never changes hits or consumes cards. */
 clean();for(unsigned t=0;t<4;t++)obv_drums_preset(&live_p[t],0);render(3);pat_init(&pat);
 for(unsigned t=0;t<4;t++){drum_pattern_set(&pat,t,0,255);seq_sound(t,&pat.st[t][0],0);}
 render(1);assert(!gates());for(unsigned t=0;t<4;t++)assert(drums_busy(t));
 page=PG_DSEQ;seq_view=SV_TRANSPOSE;seq_trk=2;transpose_keys=1;capture(47);assert(ctl_seq_transpose==-1&&!gates());
 obv_analog_choose(2);obv_context_refresh();assert(page==PG_SEQ&&seq_view==SV_TRANSPOSE);render(3);
 /* Deferred engine-transition sequence notes keep their sequence ownership. */
 obv_drums_choose(2,0);render(3);obv_analog_choose(2);seq_voice_trigger(2,70,100);render(3);
 unsigned owned=0;for(unsigned i=0;i<NV;i++)if(vc[i].gate&&vc[i].part==2){owned++;assert(vc[i].seq_owned&&ui_vnote[i]==69);}
 assert(owned);seq_voice_release(2,70,1);assert(!gates());
 /* Mono/unison legato carries sequence ownership, releases canonical notes. */
 for(unsigned mode=M_MONO;mode<=M_UNISON;mode++){
  clean();live_p[2].v[P_MODE]=mode;seq_voice_trigger(2,60,100);seq_voice_trigger(2,62,100);render(1);
  for(unsigned i=0;i<NV;i++)if(vc[i].gate){assert(vc[i].seq_owned&&ui_vnote[i]==61);}
  seq_voice_release(2,62,1);assert(!gates());
 }
 for(unsigned i=0;i<4;i++)assert(host_write_count[i]==(i==2?1u:0u));
 page=PG_SEQ;seq_view=SV_TRANSPOSE;transpose_keys=1;ctl_seq_transpose=2;draw_seq_top();draw_seq_transpose();draw_seq_knobs();write_ppm_as("build/ad1-transpose.ppm");
 puts("PASS: two engines; legacy tag fallback; four-part retune; immutable notes; ties and canonical releases; live isolation; key capture/record isolation; focus/exit/retrigger releases; reset/bounds; pattern/transport latch; DRUM-X exemption; deferred sequence ownership; mono/unison.");
}
