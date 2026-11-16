/* SPDX-License-Identifier: GPL-3.0-only */
/* DC1: transactional DRUM-X worker. CPU0 alone owns events, patches, MIDI,
 * voices, mixer and FX. CPU1 renders snapshots into private buffers. Commit
 * only after successful completion; a rejected/failed job is re-rendered from
 * the untouched original on CPU0. No extra block latency or DSP changes. */
#if OBV_DUAL_CORE
#ifdef OBV_HOST
#ifdef DC_TEST_BACKEND
static int dc_host_run(void (*fn)(uint32_t), uint32_t arg);
static int dc_host_wait(void);
#define DC_RUN(f,a) dc_host_run(f,a)
#define DC_WAIT() dc_host_wait()
#else
static int dc_sync_run(void (*fn)(uint32_t),uint32_t arg){fn(arg);return 1;}
#define DC_RUN(f,a) dc_sync_run(f,a)
#define DC_WAIT() 0
#endif
#define DC_TICKS() 0u
#define DC_READY() 1
#define DC_POOL
#else
#define DC_RUN(f,a) fm1_mc_run(f,a)
#define DC_WAIT() fm1_mc_wait()
#define DC_TICKS() fm1_ticks()
#define DC_READY() fm1_mc_on
#define DC_POOL __attribute__((section(".pool"),aligned(64)))
#endif
static volatile uint8_t dc_enabled=1;
static volatile uint32_t dc_jobs,dc_fallbacks,dc_worker_peak_ticks,dc_wait_peak_ticks;
static struct {
    obv_dhit_t hits[NPART][8];
    uint8_t params[NPART][NPARAM];
    uint16_t peaks[NPART][8];
    int32_t out[NPART][2u*CTL];
    uint32_t used,ticks;
} dc_work DC_POOL;
static void dc_worker(uint32_t mask)
{
    uint32_t t0=DC_TICKS(),used=0;
    for(unsigned p=0;p<NPART;p++)if(mask&(1u<<p)){
        memset(dc_work.out[p],0,sizeof dc_work.out[p]);
        if(drums_render_state(dc_work.hits[p],dc_work.params[p],dc_work.peaks[p],dc_work.out[p]))used|=1u<<p;
    }
    dc_work.used=used;dc_work.ticks=DC_TICKS()-t0;
}
static uint32_t dc_begin(void)
{
    uint32_t mask=0;
    if(!dc_enabled||!DC_READY())return 0;
    /* One busiest kit on CPU1; CPU0 can concurrently render the remaining
     * kits as well as melodic cards. Never serialize all drum tracks on CPU1. */
    unsigned pick=NPART,best=0;
    for(unsigned p=0;p<NPART;p++)if(obv_engine_track[p].active==OBV_DRUMS){
        unsigned cost=0;
        for(unsigned l=0;l<8;l++)cost+=!!(drum_hit[p][l].core.active||drum_hit[p][l].core.trig);
        if(cost>best){best=cost;pick=p;}
    }
    if(pick<NPART){mask=1u<<pick;
        memcpy(dc_work.hits[pick],drum_hit[pick],sizeof drum_hit[pick]);
        memcpy(dc_work.params[pick],live_p[pick].v,NPARAM);
    }
    if(!mask)return 0;
    if(!DC_RUN(dc_worker,mask)){dc_fallbacks++;return 0;}
    dc_jobs++;return mask;
}
static uint32_t dc_finish(uint32_t mask,int32_t buses[NPART][2u*HALF_FRAMES],uint32_t b)
{
    uint32_t used=0,t0;
    if(!mask)return 0;
    t0=DC_TICKS();
    if(DC_WAIT()!=0){
        dc_fallbacks++;
        for(unsigned p=0;p<NPART;p++)if(mask&(1u<<p))
            if(drums_render(p,buses[p]+2u*b))used|=1u<<p;
        return used;
    }
    uint32_t wait=DC_TICKS()-t0;
    if(wait>dc_wait_peak_ticks)dc_wait_peak_ticks=wait;
    if(dc_work.ticks>dc_worker_peak_ticks)dc_worker_peak_ticks=dc_work.ticks;
    for(unsigned p=0;p<NPART;p++)if(mask&(1u<<p)){
        memcpy(drum_hit[p],dc_work.hits[p],sizeof drum_hit[p]);
        for(unsigned l=0;l<8;l++)drum_peak[p][l]=dc_work.peaks[p][l];
        for(unsigned i=0;i<2u*CTL;i++)buses[p][2u*b+i]+=dc_work.out[p][i];
    }
    return dc_work.used;
}
#else
#define dc_begin() 0u
#define dc_finish(mask,buses,b) 0u
#endif
