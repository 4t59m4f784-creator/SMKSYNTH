/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int peak(int32_t*out){int p=0;for(unsigned i=0;i<CTL*2;i++){int a=out[i]<0?-out[i]:out[i];if(a>p)p=a;}return p;}
int main(void){int32_t out[HALF_FRAMES*2];engine_init();for(unsigned t=0;t<4;t++)trk_load(t,6);
 /* A soft hit after a fully decayed loud one must be soft in every instrument. */
 for(unsigned kit=0;kit<5;kit++)for(unsigned l=0;l<8;l++){
  obv_drums_preset(&live_p[0],kit);drums_reset(0);drums_trigger(0,DRUM_NOTE[l],127);
  for(unsigned b=0;b<25000;b++){memset(out,0,sizeof out);drums_render(0,out);if(!drums_busy(0))break;}
  assert(!drums_busy(0));drums_trigger(0,DRUM_NOTE[l],8);memset(out,0,sizeof out);drums_render(0,out);assert(peak(out)<256);
  /* Reducing a sound's level mid-hit is a negative gain ramp, not an unsigned wrap. */
  drums_reset(0);drums_trigger(0,DRUM_NOTE[l],127);for(unsigned b=0;b<4;b++){memset(out,0,sizeof out);drums_render(0,out);}
  live_p[0].v[DP_LEVEL[l]]=0;memset(out,0,sizeof out);drums_render(0,out);assert(peak(out)<100000);assert(drum_hit[0][l].gain==0);
  memset(out,0,sizeof out);drums_render(0,out);assert(!peak(out));
 }
 puts("PASS: 40 kit/instrument loud-to-soft retriggers bounded; 40 negative level ramps bounded and silent after settling.");
 /* Physical mod wheel: actual ADC mapper, two VOICE parts, focus changes and return. */
 obv_voice_preset(&live_p[0],0);obv_voice_preset(&live_p[1],1);seq_trk=0;obv_context_refresh();fm1_in.frames=1000;fm1_in.wheel[0]=535;fm1_in.wheel[1]=620;fm1_ms=3000;smk_wheels();
 fm1_in.wheel[1]=500;smk_wheels();int held=ctl_formant_wheel[0];assert(held>90);
 trk_select(1);smk_wheels();assert(ctl_formant_wheel[0]==held&&ctl_formant_wheel[1]==0);
 fm1_in.wheel[1]=550;smk_wheels();int second=ctl_formant_wheel[1];assert(second>0&&second<held&&ctl_formant_wheel[0]==held);
 trk_select(2);smk_wheels();assert(ctl_formant_wheel[0]==held&&ctl_formant_wheel[1]==second);
 trk_select(0);smk_wheels();assert(ctl_formant_wheel[0]==held); /* no takeover by a stationary wheel */
 fmt_state_t *s=&fmt_state[0];vc[0].part=0;vc[0].vel=100;vc[0].inc1=pitch_inc(60*256);vc[0].pitch16=60<<16;vc[0].amp0=vc[0].amp1=32767;fmt_start(&vc[0]);
 for(unsigned b=0;b<500;b++){memset(out,0,sizeof out);fmt_render(&vc[0],out);}int pos=s->pos;assert(pos>20000);
 trk_select(2);smk_wheels();for(unsigned b=0;b<500;b++){memset(out,0,sizeof out);fmt_render(&vc[0],out);}assert(s->pos>=pos-1&&s->pos<=pos+1);
 fm1_in.wheel[1]=620;smk_wheels();assert(ctl_formant_wheel[0]==held);trk_select(0);smk_wheels();assert(ctl_formant_wheel[0]==held);
 fm1_in.wheel[1]=615;smk_wheels();assert(ctl_formant_wheel[0]==held); /* still in deadzone, no mapped movement */
 fm1_in.wheel[1]=600;smk_wheels();assert(ctl_formant_wheel[0]<held);
 for(unsigned k=0;k<4;k++)assert(!host_write_count[k]);
 puts("PASS: real wheel mapper and formant renderer keep two independent VOICE morph values across part/focus changes; stationary wheel does not overwrite; ANALOG focus leaves VOICE state intact; no flash writes.");return 0;
}
