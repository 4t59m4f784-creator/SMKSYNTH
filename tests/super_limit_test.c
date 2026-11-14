#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t out[HALF_FRAMES*2];
static void run(void){memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);assert(super_count()<=5);}
int main(void){engine_init();pf_init();pat_init(&pat);set.tempo=120;set.m_vol=100;ctl_trk_on=15;
for(unsigned t=0;t<4;t++){patch_from_factory(&live_p[t],&FACT_INIT);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_ARP_MODE]=0;live_p[t].v[P_O1_WAVE]=W_SUPER;live_p[t].v[P_AR]=127;}
for(unsigned i=0;i<30;i++){note_event_p(i%4,1,48+i,100);run();assert(super_count()<=5);note_event_p(i%4,0,48+i,0);run();}
for(unsigned v=0;v<NV;v++){vc[v].gate=1;vc[v].ae.stage=E_REL;vc[v].ae.level=100+v;vc[v].part=v%4;vc[v].seq=v;}vc[3].gate=0;vc[3].ae.level=1;assert(super_victim()==3);run();assert(super_count()==5);
live_p[0].v[P_MODE]=M_UNISON;live_p[0].v[P_UNI_N]=8;note_event_p(0,1,80,100);run();assert(grp_n[0]<=5&&super_count()<=5);
smk_panel_panic();run();for(unsigned t=0;t<4;t++)assert(!guard_left[t]);
for(unsigned t=0;t<4;t++){live_p[t].v[P_O1_WAVE]=W_SAW;live_p[t].v[P_MODE]=M_POLY;}
for(unsigned i=0;i<8;i++){note_event_p(0,1,48+i,100);run();}unsigned n=0;for(unsigned v=0;v<NV;v++)n+=vc[v].gate;assert(n==8);
puts("PASS: five SUPER cards including release, four parts, long tails/repeated bursts, quiet released priority, UNISON cap, patch-switch guard, panic tails clear; ordinary SAW retains eight voices");}
