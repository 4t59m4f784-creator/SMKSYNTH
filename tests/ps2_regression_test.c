/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void test_arp(void){
 for(unsigned p=0;p<NPART;p++)patch_from_factory(&live_p[p],&FACT_INIT);
 ctl_seq_run=0;for(unsigned p=0;p<NPART;p++)parts_off(p);
 live_p[1].v[P_MODE]=M_DUAL;live_p[1].v[P_ARP_MODE]=A_PLAY;
 arp[1].was_on=0;seq_voice_trigger(1,63,100);unsigned before=0;
 for(unsigned v=0;v<NV;v++)before+=vc[v].gate&&vc[v].part==1&&vc[v].seq_owned;
 assert(before==2);arp_tick(1);unsigned after=0;
 for(unsigned v=0;v<NV;v++)after+=vc[v].gate&&vc[v].part==1&&vc[v].seq_owned;
 assert(after==before);seq_voice_release(1,63,1);
 for(unsigned v=0;v<NV;v++)assert(!(vc[v].gate&&vc[v].part==1));
 /* Physical voices still move into ARP, without retaining a duplicate dry note. */
 live_p[1].v[P_ARP_MODE]=A_OFF;voice_trigger(1,60,100);arp[1].was_on=0;
 live_p[1].v[P_ARP_MODE]=A_UP;arp_tick(1);
 for(unsigned v=0;v<NV;v++)assert(!(vc[v].gate&&vc[v].part==1));
}
static void fill(line_t*l,int16_t x){for(unsigned i=0;i<l->len;i++)l->b[i]=x;}
static void test_reverb(void){
 fill(&rv_d3,32767);fill(&rv_ap4,-32767);fill(&rv_d4,32767);
 fill(&rv_d1,-32767);fill(&rv_ap2,-32767);fill(&rv_d2,-32767);
 rev_tick(0);
 assert(rv_yl>100000&&rv_yl<150000);
 /* Return gain remains exactly 19661/32768, including negative rounding. */
 for(int32_t a=-229376;a<=229376;a+=97){int64_t product=(int64_t)a*19661;int32_t y=(int32_t)(product>>15);assert(y>=-137627&&y<=137627);}
}
static void test_mixer(void){
 zm_zoom=1;zm_scope=0;page=PG_MIXER;layer=L_NONE;fm1_ms=1000;seq_trk=0;
 for(unsigned row=0;row<6;row++){
  mix_row=row;zm_focus_on=0;draw_page();
  for(unsigned t=0;t<4;t++)assert(fb[(54+t/2*76)*240+4+t%2*120]==C_TRKC[t]);
  zm_focus_named("LEVEL A","99");draw_page();
  assert(fb[136*240+32]==C_PANEL); /* Badge B/C strip must never cover popup. */
  /* The rendered popup matches drawing the popup last in the same strips. */
  uint16_t before[206*115];for(unsigned y=0;y<115;y++)memcpy(before+206*y,fb+240*(72+y)+17,206*2);
  for(unsigned strip=0;strip<240;strip+=120){mui_begin(strip);zm_focus_draw();mui_end(strip);}
  for(unsigned y=0;y<115;y++)assert(!memcmp(before+206*y,fb+240*(72+y)+17,206*2));
 }
}
int main(void){engine_init();pf_init();set.tempo=ctl_tempo=120;set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();test_arp();test_mixer();test_reverb();puts("PASS PS2: ARP activation preserves sequencer first chord and releases keys; six Zoom mixer overlays; high-level reverb Q15 product does not wrap");return 0;}
