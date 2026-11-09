/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>

static int32_t audio[2*HALF_FRAMES];
static void render(void){memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);}
static void init(void){
    memset(vc,0,sizeof vc);memset(super_state,0,sizeof super_state);
    memset(&set,0,sizeof set);engine_init();pf_init();pat_init(&pat);
    seq_trk=0;layer=L_NONE;rec=REC_OFF;ctl_seq_run=0;ctl_cc7=100;ctl_pat_swap=0;ctl_mx_swap=0;nq_r=nq_w=0;ctl_panic=0;
    for(unsigned t=0;t<NTRK;t++){
        patch_from_factory(&live_p[t],&FACT_INIT);
        live_p[t].v[P_VINTAGE]=0;live_p[t].v[P_ARP_MODE]=A_OFF;
        live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_GLIDE]=0;
        set.mix_lvl[t]=100;
    }
    set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();
}
static void storage(void){
    _Static_assert(sizeof(patch_t)==80,"legacy patch");
    _Static_assert(sizeof(pattern_t)==14476,"legacy pattern");
    _Static_assert(sizeof(psnd_t)==380,"legacy scene");
    patch_t p,q;patch_defaults(&p);q=p;
    assert(patch_super_mix(&p,0)==64&&!memcmp(&p,&q,sizeof p));
    patch_super_mix_set(&p,0,7);patch_super_mix_set(&p,1,117);
    assert(patch_super_valid(&p)&&obv_patch_engine(&p)==OBV_ANALOG);
    assert(patch_super_mix(&p,0)==7&&patch_super_mix(&p,1)==117);
    assert(!memcmp(p.v,q.v,OBV_ENGINE_TAG_AT));
    p.v[79]^=1;assert(!patch_super_valid(&p)&&patch_super_mix(&p,0)==64);
    patch_super_mix_set(&p,0,0);patch_super_mix_set(&p,1,127);
    p.v[P_O1_WAVE]=p.v[P_O2_WAVE]=W_SUPER;patch_keyboard_octave_set(&p,2);
    memcpy(p.v+NPARAM,"SS1 TEST  ",10);
    prog_put(42,&p);prog_get(42,&q);assert(!memcmp(&p,&q,sizeof p));
    live_p[0]=p;trk_prog[0]=42;pf_snapshot(&pf_store.slot[0],0);pf_store.used=1;
    assert(pf_valid(&pf_store));live_p[0]=q;live_p[0].v[1]=0;pf_recall(0,0);
    assert(!memcmp(live_p[0].v,p.v,P_CHO_RATE)&&patch_super_mix(&live_p[0],1)==127);
    snd_capture(0);assert(!slot_save(0,&pat,sizeof pat,&psnd[0],sizeof psnd[0]));slot_saved(0,&pat);
    patch_defaults(&live_p[0]);pat_go(0);
    assert(patch_super_mix(&live_p[0],0)==0&&patch_super_mix(&live_p[0],1)==127);
    assert(live_p[0].v[P_O1_WAVE]==W_SUPER&&patch_keyboard_octave(&live_p[0])==2);
    assert(!memcmp(live_p[0].v+NPARAM,"SS1 TEST  ",10));
    obv_drums_preset(&q,0);p=q;patch_super_mix_set(&q,0,100);assert(!memcmp(&p,&q,sizeof p));
}
static void dsp(void){
    assert(super_curve(0)==0&&super_curve(127)<=65536);
    for(unsigned i=1;i<128;i++)assert(super_curve(i)>super_curve(i-1));
    for(unsigned note=0;note<128;note++)for(unsigned d=0;d<128;d+=127){
        osc_t c={0x12345678u,0},ref=c;super_state_t ss={0};int32_t out[CTL];
        uint32_t inc=pitch_inc(note*256);if(inc>0x70000000u)inc=0x70000000u;
        for(unsigned b=0;b<4;b++){
            super_render(&ss,&c,inc,d,0,out,CTL);
            for(unsigned i=0;i<CTL;i++)assert(out[i]==osc_step(&ref,inc,inc>>15,0,OSC_SAW,-65536,-1));
        }
        for(unsigned b=0;b<40;b++){
            super_render(&ss,&c,inc,d,127,out,CTL);
            for(unsigned i=0;i<CTL;i++)assert(out[i]>=-32768&&out[i]<=32768);
        }
    }
}
static void controls(void){
    page=PG_OSC1;set_param(P_O1_WAVE,W_SUPER);set_param(P_O2_WAVE,W_SUPER);
    assert(!strcmp(obv_voice_label(P_O1_PW),"DETUNE"));assert(live.v[P_O2_WAVE]==W_BOTH);assert(obv_param_max(&live,P_O2_WAVE)==W_BOTH);
    unsigned seq[]={PG_OSC2,PG_OSCMOD,PG_FILTER,PG_FILTER2,PG_SUPER,PG_OSC1};
    for(unsigned i=0;i<6;i++){page=super_edit_page();assert(page==seq[i]);}
    page=PG_SUPER;unsigned olddet=live.v[P_O1_PW],oldmix=patch_super_mix(&live,0);
    host_enc[ENC_KNOB2]=1;panel_encoders();
    assert(patch_super_mix(&live,0)==oldmix+2&&live.v[P_O1_PW]==olddet);
    host_enc[ENC_KNOB1]=1;panel_encoders();assert(live.v[P_O1_PW]==olddet+2);
    patch_t before=live;host_enc[ENC_KNOB3]=1;host_enc[ENC_KNOB4]=1;panel_encoders();assert(!memcmp(&before,&live,sizeof before));
    live.v[P_O2_WAVE]=W_SUPER;before=live;block_globals(&pt[0]);assert(pt[0].w2==W_SAW&&!memcmp(&before,&live,sizeof before));char text[16];param_value(text,P_O2_WAVE,W_SUPER);assert(!strcmp(text,"SAW"));
    draw_header();draw_page();draw_scope();hints_key=~0u;draw_hints();write_ppm_as("build/supersaw/super-page.ppm");
    page=PG_OSC1;draw_page();write_ppm_as("build/supersaw/osc1-page.ppm");
    obv_drums_choose(0,0);page=PG_SUPER;obv_context_refresh();assert(page==PG_DKIT);
}
static void stress(void){
    for(unsigned t=0;t<4;t++){
        live_p[t].v[P_O1_WAVE]=live_p[t].v[P_O2_WAVE]=W_SUPER;
        patch_super_mix_set(&live_p[t],0,127);patch_super_mix_set(&live_p[t],1,127);
        live_p[t].v[P_O1_PW]=live_p[t].v[P_O2_PW]=127;
        live_p[t].v[P_SYNC]=1;live_p[t].v[P_XMOD]=127;
    }
    uint64_t energy=0;
    for(unsigned n=0;n<8;n++)voice_trigger(n%4,48+n*3,100);
    for(unsigned b=0;b<200;b++){
        render();for(unsigned i=0;i<2*HALF_FRAMES;i++)energy+=(uint64_t)((int64_t)audio[i]*audio[i]);
    }
    assert(energy>1000000);
    /* First prototype bypasses SYNC/XMOD when either oscillator is SUPER. */
    voice_t saved=vc[0];super_state_t state=super_state[0];super_plain_state_t plain=super_plain_state[0];
    int32_t a[CTL*2]={0},b[CTL*2]={0};voice_render(&vc[0],a);
    vc[0]=saved;super_state[0]=state;super_plain_state[0]=plain;
    pt[0].sync_on=0;pt[0].xmod_q=0;voice_render(&vc[0],b);assert(!memcmp(a,b,sizeof a));
}
int main(void){init();storage();dsp();init();controls();init();stress();
    puts("PASS SP1: OSC1-only controls, all 128 MIDI pitches/extremes, MIX=0 exact saw, bounded output, independent knobs, page/drum routing, presets/favorites/flash-scene recall/name/octave/layout, 8-voice/4-part legacy OSC2 SUPER fallback, original stored bytes retained and explicit SYNC/XMOD bypass.");return 0;}
