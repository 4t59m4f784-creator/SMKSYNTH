/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#ifndef OBV_TEST_SOURCE
#define OBV_TEST_SOURCE "../firmware/src/obervave.c"
#endif
#include OBV_TEST_SOURCE
#undef main
/* A deterministic four-track stress stream. Compare bytes against PF12. */
int main(void) {
    int32_t audio[HALF_FRAMES*2];
    engine_init();for(unsigned t=0;t<4;t++){trk_load(t,t*6);live_p[t].v[P_ARP_MODE]=A_OFF;}
    for(unsigned t=0;t<4;t++){set.mix_lvl[t]=100;set.mix_cho[t]=40;set.mix_dly[t]=30;set.mix_rev[t]=45;}
    set.m_vol=100;set.m_dj=64;fx_sync();mix_apply();pat_init(&pat);
    uint64_t audible=0;
    for(unsigned b=0;b<1500;b++) {
        unsigned t=(b/32)%4,n=48+(b/128)%24;
        if(b%32==0)note_event_p(t,1,n,70+b%50);
        if(b%32==24)note_event_p(t,0,n,0);
        if(b%150==0){ctl_wheel_part=t;ctl_wheel_bend=(int)(b%8192)-4096;ctl_vib=b%128;ctl_mix_dly[t]=b%128;}
        if(b==500){pat.st[2][0].note[0]=72;pat.st[2][4].note[0]=67;ctl_seq_run=1;}
        if(b==1100)ctl_seq_run=0;
        memset(audio,0,sizeof audio);engine_audio(audio,HALF_FRAMES);
        for(unsigned i=0;i<HALF_FRAMES*2;i++)if(audio[i])audible++;
        fwrite(audio,1,sizeof audio,stdout);
    }
    if(audible<100000){fprintf(stderr,"FAIL: audio scenario was silent or insufficient\n");return 1;}
    fprintf(stderr,"AUDIBLE: %llu nonzero samples\n",(unsigned long long)audible);
    return 0;
}
