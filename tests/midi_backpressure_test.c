/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(void){
 engine_init();pf_init();pat_init(&pat);mm_boot();fm1_ms=2000;fm1_in.frames=400;
 for(unsigned t=0;t<4;t++)trk_load(t,t);patch_t before[4];memcpy(before,live_p,sizeof before);
 host_midi_blocked=1;mm_enter();assert(mm_panic_left==80);mm_last=MM_KEYS;
 mm_cv(0,97,0);assert(mm_last==0&&mm_last_v==97);mm_cv(16,111,0);assert(mm_last==16&&mm_last_v==111);
 mm_service();assert(mm_panic_left==80&&!host_midi_count);
 host_qput(0,(8<<8)|113);mm_input();assert(mm_last==MM_PAD+8&&mm_last_v==113);host_qput(0,8<<8);mm_input();assert(mm_last==MM_PAD+8&&mm_last_v==113);

 mm_leave();fm1_in.keys=fm1_in.buttons=0;assert(!mm_return_poll());assert(!mm_on&&!mm_wait_release&&mout_blocked&&mm_panic_left==80);
 /* Local synth plays while output safety remains pending. */
 note_in(1,60,100);int32_t audio[512];memset(audio,0,sizeof audio);app_audio(audio,256);unsigned audible=0;for(unsigned i=0;i<512;i++)audible|=(unsigned)(audio[i]!=0);assert(audible);note_in(0,60,0);
 smk_midi_controls_poll();assert(mout_blocked&&mm_panic_left==80);
 host_midi_blocked=0;for(unsigned i=0;i<100;i++){fm1_ms++;mm_service();smk_midi_controls_poll();}
 assert(!mm_panic_left&&!mout_blocked);assert(!memcmp(before,live_p,sizeof before));
 /* Controls remain live and coalesce behind a stalled entry safety flush. */
 host_midi_blocked=1;mm_enter();host_midi_count=0;mm_cv(0,31,0);mm_cv(0,96,0);mm_cv(16,103,0);
 host_midi_blocked=0;for(unsigned i=0;i<100;i++){fm1_ms++;mm_service();}
 assert(!mm_panic_left&&host_midi_count==82);assert(host_midi_sent[80][0]==0xB0&&host_midi_sent[80][1]==70&&host_midi_sent[80][2]==96);assert(host_midi_sent[81][1]==20&&host_midi_sent[81][2]==103);
 puts("PASS: stalled MIDI entry keeps controls live; synth return and local audio never wait for host output; pending safety resets precede coalesced controls and new outbound notes; all patches unchanged.");
}
