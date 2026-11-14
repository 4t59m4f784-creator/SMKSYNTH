#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static int32_t out[HALF_FRAMES*2];
static void tick(unsigned n){while(n--){memset(out,0,sizeof out);engine_audio(out,HALF_FRAMES);smk_midi_controls_poll();}}
static unsigned seen(unsigned st,unsigned n){unsigned c=0;for(unsigned i=0;i<host_midi_count;i++)c+=host_midi_sent[i%256][0]==st&&host_midi_sent[i%256][1]==n;return c;}
int main(void){engine_init();pf_init();pat_init(&pat);set.tempo=120;set.m_vol=100;set.midi_ch=3;mout_base=2;ctl_trk_on=15;seq_trk=0;page=PG_SEQ;seq_view=SV_ENDLESS;layer=0;step_held=-1;
for(unsigned t=0;t<4;t++){patch_from_factory(&live_p[t],&FACT_INIT);live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_ARP_MODE]=0;pat.trk[t].len=1;pat.st[t][0].note[0]=60+t;pat.st[t][0].vel=100;}
assert(endless_enable());panel_note_in(1,62,100);tick(3);for(unsigned t=0;t<4;t++)assert(seen(0x92+t,62+t));panel_note_in(0,62,0);endless_stop();tick(3);for(unsigned t=0;t<4;t++)assert(seen(0x82+t,62+t));
host_midi_count=0;panel_note_in(1,60,100);tick(3);smk_panel_panic();tick(80);assert(!end_pending&&!end_record&&!end_held&&!ctl_seq_run);for(unsigned s=0;s<MOUT_SOURCES;s++)for(unsigned t=0;t<4;t++)for(unsigned n=0;n<128;n++)assert(!mout_owner[s][t][n]);puts("PASS: ENDLESS four channels, C4-relative transpose, paired stop releases, Panic clears launches/owners");}
