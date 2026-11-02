/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
#include "reference/jiant/vowel_fixture.h"
int main(void){
 uint64_t samples=0,nonzero=0;engine_init();
 for(unsigned p=0;p<8;p++)for(unsigned note=48;note<=84;note+=12){
  jv_state_t st={0};jv_voice_t ref={0};jv_track_t t={0};jv_mod_t m={0};
  obv_voice_preset(&live_p[0],p);assert(jv_profile(&live_p[0]));
  const uint8_t*v=live_p[0].v;t=(jv_track_t){{v[VP_VOWEL],v[VP_VOWEL2],v[VP_TALK],(int)v[VP_SHIFT]-64,v[VP_BUZZ],v[VP_BREATH],v[VP_Q],0}};
  st.v.vel=ref.vel=100;st.v.pitch_cur=ref.pitch_cur=note*16;st.v.pitch16=ref.pitch16=note*16;st.v.gfrom=ref.gfrom=note*16;
  jv_noise=0x2545F491u;ref_noise=0x2545F491;
  for(unsigned k=0;k<1000;k++){
   int32_t a[32]={0},b[32]={0};
   m.inc=jv_pitch_inc(note*16);m.pitch16=note*16;m.shape=64<<8;
   m.amp0=(k*73)%32768;m.amp1=(k*113)%32768;
   if(!(k%53)){t.p[P_E0]=(k*7)%128;t.p[P_E1]=(k*13)%128;t.p[P_E4]=(k*3)%128;t.p[P_E5]=(k*11)%128;t.p[P_E6]=40+(k%50);}
   m.cutoff=(k%80)*96;
   if(k==500){st.v.gfrom=ref.gfrom=note*16;st.v.pitch16=ref.pitch16=(note+7)*16;}
   if(k>500&&k<750){st.v.pitch_cur=ref.pitch_cur=note*16+(int32_t)(k-500)*112/249;}
   if(k==750){st.v.gfrom=ref.gfrom=(note+7)*16;st.v.pitch16=ref.pitch16=(note-5)*16;}
   if(k>=750){st.v.pitch_cur=ref.pitch_cur=(note+7)*16-(int32_t)(k-750)*192/249;}
   jv_render32(&st,&t,&st.v,a,32,&m);ref_render(&t,&ref,b,32,&m);
   for(unsigned i=0;i<32;i++){if(a[i]!=b[i]){fprintf(stderr,"mismatch p=%u note=%u block=%u sample=%u actual=%d ref=%d\n",p,note,k,i,a[i],b[i]);return 1;}samples++;nonzero+=a[i]!=0;}
  }
 }
 assert(nonzero>100000);printf("PASS: %llu exact PCM samples against pinned JIANT vowel kernel: TALK endpoints, changing vowels/BUZZ/BREATH/Q, cutoff, positive/negative amp ramps, sung glide.\n",(unsigned long long)samples);
 /* Profile 1 stays readable without any migration/write. */
 obv_voice_preset(&live_p[0],0);live_p[0].v[OBV_ENGINE_TAG_AT+3]=1;patch_t legacy=live_p[0];assert(obv_patch_engine(&legacy)==OBV_VOICE&&!jv_profile(&legacy));
 fmt_start(&vc[0]);assert(!memcmp(&legacy,&live_p[0],sizeof legacy));
 puts("PASS: legacy VOICE profile 1 accepted without rewriting patch bytes.");return 0;
}
