/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static unsigned starts, hits, resets;
static void test_start(voice_t *v) { (void)v; starts++; }
static void test_render(voice_t *v, int32_t *out) {
    (void)v; for (unsigned i=0;i<CTL*2;i++) out[i]+=2000;
}
static void test_hit(uint32_t p,uint32_t n,uint32_t v) { (void)p;(void)n;(void)v;hits++; }
static void test_reset(uint32_t p) { (void)p;resets++; }
static const obv_engine_ops_t melodic={"TEST CARD",test_start,test_render,0,0,test_reset};
static const obv_engine_ops_t percussion={"TEST HITS",0,0,test_hit,0,test_reset};
static int32_t audio[HALF_FRAMES*2];
static void render(void) { memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES); }
int main(void) {
    engine_init(); for(unsigned t=0;t<4;t++) { trk_load(t,0);live_p[t].v[P_ARP_MODE]=A_OFF; }
    fx_sync(); mix_apply(); pf_init(); pat_init(&pat);
    for(unsigned t=0;t<4;t++) assert(obv_engine_track[t].active==OBV_ANALOG);
    unsigned w=nq_w;
    assert(!obv_engine_request(4,OBV_ANALOG));assert(!obv_engine_request(0,3));
    assert(obv_engine_available(OBV_VOICE));assert(obv_engine_available(OBV_DRUMS));assert(nq_w==w);
    /* Test adapters exercise routing, not sound quality or JIANT integration. */
    obv_engines[OBV_VOICE]=&melodic;obv_engines[OBV_DRUMS]=&percussion;
    note_event_p(1,1,60,100);render();assert(nheld[1]==1);
    note_event_p(0,1,64,100);render();assert(nheld[0]==1);
    assert(obv_engine_request(1,OBV_VOICE));assert(obv_engine_track[1].active==OBV_ANALOG);
    render();assert(obv_engine_track[1].active==OBV_VOICE&&nheld[1]==0&&nheld[0]==1);
    for(unsigned v=0;v<NV;v++) if(vc[v].part==1) assert(vc[v].ae.stage==E_IDLE&&!vc[v].gate);
    assert(obv_engine_track[1].gain==0);render();assert(obv_engine_track[1].phase==OBV_ENGINE_READY);
    note_event_p(1,1,67,100);render();assert(starts==1);
    unsigned card=NV;
    for(unsigned v=0;v<NV;v++) if(vc[v].part==1&&vc[v].gate)card=v;
    assert(card<NV&&obv_voice_engine[card]==OBV_VOICE);
    assert(obv_engine_request(2,OBV_DRUMS));render();render();
    unsigned cards=0;for(unsigned v=0;v<NV;v++)cards+=vc[v].gate;
    note_event_p(2,1,36,127);render();assert(hits==1);
    unsigned after=0;for(unsigned v=0;v<NV;v++)after+=vc[v].gate;assert(cards==after);
    /* The current sequencer reaches the same dispatcher (8 lanes come later). */
    sr[2].nsnd=0;step_t step;step_clear(&step);step.note[0]=38;
    seq_sound(2,&step,0);assert(hits==2);
    assert(obv_engine_request(1,OBV_ANALOG));render();render();assert(resets==1);
    assert(obv_engine_track[1].active==OBV_ANALOG&&nheld[0]==1);
    ctl_panic=1;render();assert(resets==2); /* percussion panic has its own reset */
    /* Full ring rejects an engine command without corrupting queued notes. */
    while(nq_w-nq_r<NQ)note_event_p(0,0,64,0);
    w=nq_w;assert(!obv_engine_request(0,OBV_VOICE)&&nq_w==w);render();
    for(unsigned i=0;i<4;i++)assert(host_write_count[i]==0);
    puts("PASS: unavailable engines rejected, ISR switch/fades, track isolation, melodic dispatch, independent hit dispatch, sequencer route, ring overflow, zero flash writes.");
    return 0;
}
