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
    uint8_t params[NPART][(NPARAM+3u)&~3u] __attribute__((aligned(4)));
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
/* may_alias allows aligned word copies of object representations without
 * strict-aliasing UB. Unaligned ends safely use the original byte path. */
typedef uint32_t dc_alias_word __attribute__((may_alias));
static __attribute__((noinline)) void dc_copy(void *dst,const void *src,unsigned n)
{
    unsigned char *d=dst;const unsigned char *s=src;
    if(!(((uintptr_t)d|(uintptr_t)s)&3u)){
        while(n>=16u){
            dc_alias_word *w=(dc_alias_word*)d;const dc_alias_word *v=(const dc_alias_word*)s;
            w[0]=v[0];w[1]=v[1];w[2]=v[2];w[3]=v[3];d+=16;s+=16;n-=16;
        }
        while(n>=4u){*(dc_alias_word*)d=*(const dc_alias_word*)s;d+=4;s+=4;n-=4;}
    }
    while(n--)*d++=*s++;
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
    dp_begin(DP_COPY);
    if(pick<NPART){mask=1u<<pick;
        dc_copy(dc_work.hits[pick],drum_hit[pick],sizeof drum_hit[pick]);
        dc_copy(dc_work.params[pick],live_p[pick].v,NPARAM);
    }
    dp_end(DP_COPY);
    if(!mask)return 0;
    if(!DC_RUN(dc_worker,mask)){if(DP_RECORD)dc_fallbacks++;return 0;}
    if(DP_RECORD)dc_jobs++;return mask;
}
static uint32_t dc_finish(uint32_t mask,int32_t buses[NPART][2u*HALF_FRAMES],uint32_t b)
{
    uint32_t used=0,t0;
    if(!mask)return 0;
    t0=DC_TICKS();
    dp_begin(DP_WAIT);
    if(DC_WAIT()!=0){
        dp_end(DP_WAIT);dp_begin(DP_DRUM);
        if(DP_RECORD)dc_fallbacks++;
        for(unsigned p=0;p<NPART;p++)if(mask&(1u<<p))
            if(drums_render(p,buses[p]+2u*b))used|=1u<<p;
        dp_end(DP_DRUM);return used;
    }
    dp_end(DP_WAIT);
    uint32_t wait=DC_TICKS()-t0;
    if(DP_RECORD&&wait>dc_wait_peak_ticks)dc_wait_peak_ticks=wait;
    if(DP_RECORD&&dc_work.ticks>dc_worker_peak_ticks)dc_worker_peak_ticks=dc_work.ticks;
    dp_begin(DP_COPY);
    for(unsigned p=0;p<NPART;p++)if(mask&(1u<<p)){
        dc_copy(drum_hit[p],dc_work.hits[p],sizeof drum_hit[p]);
        for(unsigned l=0;l<8;l++)drum_peak[p][l]=dc_work.peaks[p][l];
        for(unsigned i=0;i<2u*CTL;i++)buses[p][2u*b+i]+=dc_work.out[p][i];
    }
    dp_end(DP_COPY);
    return dc_work.used;
}
#else
#define dc_begin() 0u
#define dc_finish(mask,buses,b) 0u
#endif
