/* SPDX-License-Identifier: GPL-3.0-only */
/* Compile unchanged against SS2 and SS3; compare the complete rendered stream. */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(int argc,char **argv)
{
    assert(argc==2);FILE *f=fopen(argv[1],"wb");assert(f);
    engine_init();pf_init();pat_init(&pat);seq_trk=0;ctl_cc7=100;
    for(unsigned t=0;t<4;t++){
        trk_load(t,98);set.mix_lvl[t]=100;
        live_p[t].v[P_ARP_MODE]=A_OFF;
        live_p[t].v[P_MODE]=M_POLY;live_p[t].v[P_AA]=1;live_p[t].v[P_AR]=70;
        live_p[t].v[P_VINTAGE]=30;
    }
    set.m_vol=100;set.m_dj=64;set.tempo=120;fx_sync();mix_apply();
    for(unsigned test=0;test<4;test++){
        for(unsigned t=0;t<4;t++){
            live_p[t].v[P_O1_WAVE]=W_SUPER;
            live_p[t].v[P_O2_WAVE]=test&1?W_SUPER:W_SAW;
            live_p[t].v[P_O2_LVL]=test==2?0:100;
            live_p[t].v[P_LFO_PITCH]=15;live_p[t].v[P_LFO_PWM]=30;
            set.mix_cho[t]=set.mix_dly[t]=set.mix_rev[t]=test>1?90:0;
        }
        mix_apply();
        for(unsigned attack=0;attack<12;attack++){
            unsigned count=attack<4?4:8;
            for(unsigned v=0;v<count;v++)note_event_p(v%4,1,72+v*4+(attack%4)*3,100);
            for(unsigned b=0;b<40;b++){
                int32_t out[2*HALF_FRAMES]={0};engine_audio(out,HALF_FRAMES);
                assert(fwrite(out,sizeof *out,2*HALF_FRAMES,f)==2*HALF_FRAMES);
            }
            for(unsigned v=0;v<count;v++)note_event_p(v%4,0,72+v*4+(attack%4)*3,0);
            /* Leave releases alive into the following attack. */
            for(unsigned b=0;b<8;b++){
                int32_t out[2*HALF_FRAMES]={0};engine_audio(out,HALF_FRAMES);
                assert(fwrite(out,sizeof *out,2*HALF_FRAMES,f)==2*HALF_FRAMES);
            }
        }
    }
    fclose(f);puts("PASS: high-note 4/8-note bursts, release overlap, SUPER+SAW / dual SUPER / silent OSC2, modulation/drift and FX stream rendered.");
    return 0;
}
