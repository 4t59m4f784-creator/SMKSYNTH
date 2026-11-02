/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void save(const char*n,const void*p,unsigned size){FILE*f=fopen(n,"wb");assert(f);assert(fwrite(p,1,size,f)==size);fclose(f);}
int main(void){engine_init();pf_init();pat_init(&pat);for(unsigned i=0;i<NPSECT;i++)psect[i].tag=PS_TAG;for(unsigned t=0;t<4;t++)trk_load(t,0);seq_trk=0;set.version=7;set.tempo=120;memcpy(live_p[0].v+NPARAM,"BASS WORK ",10);patch_keyboard_octave_set(&live_p[0],-2);prog_put(70,&live_p[0]);pat.st[0][0].note[0]=24;drum_pattern_set(&pat,3,0,255);snd_capture(0);pf_store.used=1;pf_store.slot[0]=pf_capture[0];pf_snapshot(&pf_store.slot[0],0);save("build/fixture-psect0.bin",&psect[0],sizeof psect[0]);save("build/fixture-psect1.bin",&psect[1],sizeof psect[1]);save("build/fixture-pattern.bin",&pat,sizeof pat);save("build/fixture-scene.bin",&psnd[0],sizeof psnd[0]);save("build/fixture-favorites.bin",&pf_store,sizeof pf_store);save("build/fixture-globals.bin",&set,sizeof set);return 0;}
