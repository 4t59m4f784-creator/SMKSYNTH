/* SPDX-License-Identifier: GPL-3.0-only */
/* DC3: transactional melodic sample rendering. CPU0 owns events, envelopes,
 * UI, allocation, drums, mixing and FX. Part globals/patches are immutable
 * from dispatch to join: main loop is suspended and Timer5 only queues input.
 * CPU1 writes private voice/oscillator states and output. A failed job commits
 * nothing and CPU0 renders from the untouched prepared original. */
#if OBV_DUAL_CORE
#ifdef OBV_HOST
#ifdef DC_TEST_BACKEND
static int dc_host_run(void (*fn)(uint32_t),uint32_t arg);
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
    voice_t voice[NV];
    super_state_t super[NV];
    super_plain_state_t plain[NV];
    int32_t out[NV][2u*CTL];
    uint32_t ticks;
} dc_work DC_POOL;
static void dc_worker(uint32_t mask)
{
    uint32_t t0=DC_TICKS();
    for(unsigned v=0;v<NV;v++)if(mask&(1u<<v)){
        memset(dc_work.out[v],0,sizeof dc_work.out[v]);
        voice_render_state(&dc_work.voice[v],&pt[dc_work.voice[v].part],
            &dc_work.super[v],&dc_work.plain[v],dc_work.out[v]);
    }
    dc_work.ticks=DC_TICKS()-t0;
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
/* Match the renderer itself, including a fading voice during engine switch. */
static unsigned dc_super(unsigned v){return pt[vc[v].part].w1==W_SUPER;}
static uint32_t dc_begin(uint32_t active)
{
    if(!dc_enabled||!DC_READY())return 0;
    unsigned total=0,selected=0;uint32_t mask=0;
    for(unsigned v=0;v<NV;v++)if((active&(1u<<v))&&!obv_engines[obv_voice_engine[v]]->render)
        total+=dc_super(v)?3u:2u;
    /* Approximate half the sample cost; prefer SUPER, but normal voices also
     * offload when no SUPER is used. Keep a single active voice on CPU0. */
    if(total<4u)return 0;
    for(unsigned pass=0;pass<2;pass++)for(unsigned v=0;v<NV;v++)
        if((active&(1u<<v))&&!obv_engines[obv_voice_engine[v]]->render&&!!dc_super(v)==!pass){
            unsigned cost=dc_super(v)?3u:2u;
            if(selected+cost>total/2u)continue;
            selected+=cost;mask|=1u<<v;
        }
    if(!mask)return 0;
    dp_begin(DP_COPY);
    for(unsigned v=0;v<NV;v++)if(mask&(1u<<v)){
        dc_copy(&dc_work.voice[v],&vc[v],sizeof vc[v]);
        /* Normal render only clears SUPER.active; no stale SUPER fields are
         * read or committed. Avoid copying the unused side-oscillator bank. */
        if(dc_super(v)){
            dc_copy(&dc_work.super[v],&super_state[v],sizeof super_state[v]);
            dc_copy(&dc_work.plain[v],&super_plain_state[v],sizeof super_plain_state[v]);
        }
    }
    dp_end(DP_COPY);
    if(!DC_RUN(dc_worker,mask)){if(DP_RECORD)dc_fallbacks++;return 0;}
    if(DP_RECORD)dc_jobs++;return mask;
}
static uint32_t dc_finish(uint32_t mask,int32_t buses[NPART][2u*HALF_FRAMES],uint32_t b)
{
    uint32_t t0;if(!mask)return 0;
    t0=DC_TICKS();dp_begin(DP_WAIT);
    if(DC_WAIT()!=0){
        dp_end(DP_WAIT);if(DP_RECORD)dc_fallbacks++;
        for(unsigned v=0;v<NV;v++)if(mask&(1u<<v)){
            unsigned slot=dc_super(v)?DP_SUPER:DP_ANALOG;
            dp_begin(slot);voice_render(&vc[v],buses[vc[v].part]+2u*b);dp_end(slot);
        }
        return 0;
    }
    dp_end(DP_WAIT);uint32_t wait=DC_TICKS()-t0;
    if(DP_RECORD&&wait>dc_wait_peak_ticks)dc_wait_peak_ticks=wait;
    if(DP_RECORD&&dc_work.ticks>dc_worker_peak_ticks)dc_worker_peak_ticks=dc_work.ticks;
    dp_begin(DP_COPY);
    for(unsigned v=0;v<NV;v++)if(mask&(1u<<v)){
        dc_copy(&vc[v],&dc_work.voice[v],sizeof vc[v]);
        if(dc_super(v)){
            dc_copy(&super_state[v],&dc_work.super[v],sizeof super_state[v]);
            dc_copy(&super_plain_state[v],&dc_work.plain[v],sizeof super_plain_state[v]);
        }else super_state[v].active=0;
        for(unsigned i=0;i<2u*CTL;i++)buses[vc[v].part][2u*b+i]+=dc_work.out[v][i];
    }
    dp_end(DP_COPY);return 0;
}
#else
#define dc_begin(active) 0u
#define dc_finish(mask,buses,b) 0u
#endif
