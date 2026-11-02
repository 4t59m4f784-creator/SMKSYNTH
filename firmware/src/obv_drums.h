/* SPDX-License-Identifier: GPL-3.0-only */
/* Independent fixed hit pool: one retriggerable hit per instrument per track.
 * Audio-thread owned; no melodic cards, malloc, samples or own FX chain. */
#include "obv_drumx_core.h"
#include "obv_drum_kits.h"
static const uint8_t DRUM_NOTE[8]={36,38,39,42,46,45,37,56};
static const char *const DRUM_NAME[8]={"KICK","SNARE","CLAP","HAT CL","HAT OP","TOM","RIM","BELL"};
static const uint8_t DP_LEVEL[8]={P_O1_PW,P_O1_LVL,P_NOISE,P_O2_PW,P_O2_LVL,P_XMOD,P_ENV_O2,P_O2_FINE};
static const uint8_t DP_DECAY[8]={P_FA,P_FD,P_FS,P_FR,P_AA,P_AD,P_AS,P_AR};
static const uint8_t DP_PITCH[8]={P_LFO_PITCH,P_LFO_PWM,P_LFO_FLT,P_LFO_AMP,P_KEYTRK,P_VEL_FLT,P_VEL_AMP,P_VINTAGE};
#define DP_MORPH P_CUTOFF
#define DP_TONE P_RESO
#define DP_LENGTH P_FENV_AMT
#define DP_FOLD P_DRIVE
typedef struct {dx_voice_t core;uint8_t velocity,kit;int32_t gain;} obv_dhit_t;
static obv_dhit_t drum_hit[NPART][8];
static volatile uint16_t drum_peak[NPART][8];
static volatile uint32_t drum_stamp[NPART][8];
static uint8_t drum_ui_lane[NPART];
static int drum_lane(uint32_t note)
{
    if(note>=60u&&note<68u)return (int)note-60;
    for(unsigned i=0;i<8;i++)if(DRUM_NOTE[i]==note)return (int)i;
    return -1;
}
static uint32_t drums_busy(uint32_t p)
{
    for(unsigned l=0;l<8;l++)if(drum_hit[p][l].core.active||drum_hit[p][l].core.trig)return 1;
    return 0;
}
static void drums_reset(uint32_t p)
{
    for(unsigned l=0;l<8;l++){
        memset(&drum_hit[p][l],0,sizeof drum_hit[p][l]);drum_peak[p][l]=0;drum_stamp[p][l]=0;
    }
}
static void drums_choke(uint32_t p,uint32_t l)
{
    dx_voice_t *v=&drum_hit[p][l].core;
    v->trig=0;dx_choke(v); /* includes a not-yet-rendered open-hat hit */
}
static void drums_trigger(uint32_t p,uint32_t note,uint32_t vel)
{
    int ln=drum_lane(note);if(ln<0||!vel)return;
    uint32_t l=(uint32_t)ln;
    if(!live_p[p].v[DP_LEVEL[l]])return;
    if(l==DV_HATC)drums_choke(p,DV_HATO);
    if(l==DV_HATO)drums_choke(p,DV_HATC);
    obv_dhit_t *h=&drum_hit[p][l];
    h->velocity=(uint8_t)(vel>127?127:vel);
    h->kit=(uint8_t)(live_p[p].v[73]%NDRUMKIT);
    if(!h->core.rng)h->core.rng=(int32_t)(0x2545F491u^((p*8u+l+1u)*0x9E3779B9u));
    dx_trigger(&h->core);drum_stamp[p][l]=eng_t+1u;
}
/* The 606/707 labels are timbral starting points, not emulations of their ROMs. */
static void drums_lane_config(uint32_t kit,uint32_t lane,dx_lane_t *out)
{
    *out=DRUM_KITS[kit%NDRUMKIT].lane[lane];
    if(kit==2u){
        for(unsigned p=0;p<2;p++){uint8_t *v=p?out->b:out->a;
            if(lane==DV_KICK){v[DXP_PITCH]=14;v[DXP_DECAY]=52;v[DXP_PMOD]=50;v[DXP_NOISE]=0;}
            if(lane==DV_SNARE){v[DXP_DECAY]=42;v[DXP_NOISE]=95;v[DXP_COLOR]=92;}
            if(lane==DV_HATC){v[DXP_DECAY]=12;v[DXP_COLOR]=110;}
            if(lane==DV_HATO){v[DXP_DECAY]=48;v[DXP_COLOR]=106;}
        }
    }else if(kit==3u){
        for(unsigned p=0;p<2;p++){uint8_t *v=p?out->b:out->a;
            if(lane==DV_KICK){v[DXP_PITCH]=12;v[DXP_PMOD]=100;v[DXP_DECAY]=60;v[DXP_COLOR]=60;}
            if(lane==DV_SNARE){v[DXP_DECAY]=48;v[DXP_NOISE]=110;v[DXP_COLOR]=115;}
            if(lane==DV_CLAP){v[DXP_DECAY]=40;v[DXP_COLOR]=100;}
            if(lane==DV_HATC){v[DXP_DECAY]=17;v[DXP_NOISE]=116;v[DXP_COLOR]=124;}
            if(lane==DV_HATO){v[DXP_DECAY]=58;v[DXP_COLOR]=120;}
        }
    }
}
static uint32_t drums_render(uint32_t p,int32_t *out)
{
    uint32_t used=0;const uint8_t *v=live_p[p].v;
    int32_t y[CTL];
    for(unsigned l=0;l<8;l++){
        obv_dhit_t *h=&drum_hit[p][l];drum_peak[p][l]=0;
        if(!h->core.active&&!h->core.trig)continue;
        used=1;uint32_t retrigger=h->core.trig,kit=h->kit;dx_lane_t lane;drums_lane_config(kit,l,&lane);
        int32_t tune=((int32_t)v[P_O2_SEMI]-24)*16+clampi((int32_t)v[DP_PITCH[l]]-64,-12,12)*16;
        dx_run(&lane,&h->core,v[DP_MORPH],tune,
            (int32_t)v[DP_LENGTH]-64+(int32_t)v[DP_DECAY[l]]-64,(int32_t)v[DP_TONE]-64,0,
            v[DP_FOLD],0,y,CTL,l,DRUM_KITS[kit].tone,DRUM_KITS[kit].hands,DRUM_KITS[kit].layer[l]);
        int32_t target=(int32_t)((h->velocity*h->velocity*4096u)/16129u);
        target=(int32_t)(((int64_t)target*v[DP_LEVEL[l]]*v[DP_LEVEL[l]])/10000);
        target=(int32_t)(((int64_t)target*v[P_VOLUME]*v[P_VOLUME])/16384);
        /* A fresh hit has its own attack gain; never inherit the previous
         * velocity. In particular, loud -> soft must stay soft from sample 1. */
        int32_t g=retrigger?0:h->gain,dg=target-g;h->gain=target;
        for(unsigned i=0;i<CTL;i++){
            int32_t a=(int32_t)(((int64_t)y[i]*(g+(dg*(int32_t)(i+1u))/(int32_t)CTL))>>12);
            int32_t pk=a<0?-a:a;if(pk>32767)pk=32767;
            if(pk>drum_peak[p][l])drum_peak[p][l]=(uint16_t)pk;
            out[2*i]+=a;out[2*i+1]+=a;
        }
    }
    return used;
}
static const obv_engine_ops_t obv_drums_ops={"DRUMS",0,0,drums_trigger,drums_render,drums_reset,drums_busy};
static void obv_drums_preset(patch_t *p,uint32_t kit)
{
    patch_defaults(p);kit%=NDRUMKIT;
    for(unsigned l=0;l<8;l++){p->v[DP_LEVEL[l]]=100;p->v[DP_DECAY[l]]=64;p->v[DP_PITCH[l]]=64;}
    p->v[DP_MORPH]=0;p->v[DP_TONE]=64;p->v[DP_LENGTH]=64;p->v[DP_FOLD]=0;
    p->v[P_MODE]=M_POLY;p->v[P_ARP_MODE]=A_OFF;p->v[P_O2_SEMI]=24;p->v[P_VOLUME]=100;
    for(unsigned i=0;i<PATCH_NAME&&DRUM_KIT_NAME[kit][i];i++)p->v[NPARAM+i]=(uint8_t)DRUM_KIT_NAME[kit][i];
    p->v[73]=(uint8_t)kit;obv_patch_set_engine(p,OBV_DRUMS);
}
