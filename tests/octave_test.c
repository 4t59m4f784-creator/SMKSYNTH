#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t audio[HALF_FRAMES*2];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}
int main(void){engine_init();pf_init();pat_init(&pat);for(unsigned t=0;t<4;t++)trk_load(t,0);
assert(NPARAM+PATCH_NAME<=PATCH_KBO_AT);assert(sizeof(patch_t)==80&&sizeof(psnd_t)==380);
seq_trk=0;panel_octave_set(-2);seq_trk=1;panel_octave_set(1);assert(panel_octave()==1);seq_trk=0;assert(panel_octave()==-2);
panel_note_in(1,60,100);render();assert(kb_press[60].route[0].note==36);seq_trk=1;panel_note_in(1,48,100);assert(kb_press[48].route[0].note==60&&kb_press[60].count==1);panel_note_in(0,60,0);panel_note_in(0,48,0);assert(!kb_press[60].count&&!kb_press[48].count);
keyboard_cfg_t k;keyboard_defaults(&k);k.mode=KB_LAYER;k.octave[0]=1;pat.keyboard=k;panel_note_in(1,48,100);assert(kb_press[48].route[0].note==36&&kb_press[48].route[1].note==60);panel_note_in(0,48,0);
seq_trk=0;prog_put(20,&live_p[0]);panel_octave_set(2);trk_load(0,20);assert(panel_octave()==-2);pf_store.slot[0].patch=live_p[0];pf_store.used=1;seq_trk=1;pf_recall(0,1);assert(panel_octave()==-2);
snd_capture(0);psnd_t saved=psnd[0];pattern_focus_set(&pat,0);assert(!pattern_slot_save(0,&pat,&saved));panel_octave_set(2);load_pattern(0);assert(patch_keyboard_octave(&live_p[0])==-2&&patch_keyboard_octave(&live_p[1])==-2);
pat_init(&pat);seq_trk=0;live_p[0].v[P_ARP_MODE]=A_OFF;panel_octave_set(-2);rec=REC_READY;ctl_seq_run=0;fm1_ms=100;panel_note_in(1,48,100);render();assert(pat.st[0][0].note[0]==24);fm1_ms+=200;panel_note_in(0,48,0);rec=REC_OFF;ctl_seq_run=0;
patch_t old;patch_from_factory(&old,&FACT_INIT);assert(patch_keyboard_octave(&old)==0);
puts("PASS: per-part octave, physical routes, held notes across octave/part changes, layer additive offsets, preset/favorite/scene roundtrips, legacy0, record sizes unchanged.");}
