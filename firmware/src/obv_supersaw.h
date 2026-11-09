/* SPDX-License-Identifier: GPL-3.0-only */
/* SP1: seven saws inside one oscillator. Offsets and detune curve adapted
 * from isod89/clipper-fm1 eng_hybrid.c (3dSam / Leo Kuroshita, 2026),
 * itself referencing Adam Szabo's JP-8000 analysis. Independent MIX and
 * peak-normalized gains are SMKSYNTH additions; this is not a JP emulator.
 * Uses our event-corrected saw, including its one-sample delay. */
typedef struct {
    osc_t side[6];
    int32_t det_sm, mix_sm;               /* Q8 controls; slew only while SUPER */
    uint8_t active;
    uint32_t inc_cached, det_cached, si[6];
    int32_t det_offset[6]; /* exact off[j]*curve, changes only with detune */
    int32_t mix_cached, cg;
    uint32_t reciprocal[7];
} super_state_t;
typedef struct {uint32_t inc_cached, reciprocal;} super_plain_state_t;

#if (defined(OBV_HOST) && !defined(OBV_SUPER_EXTERNAL)) || defined(OBV_SUPER_KERNEL)
static inline __attribute__((always_inline)) uint32_t super_curve(uint32_t shape)
{
    int32_t x = (int32_t)shape * 516;
    return (uint32_t)((((int64_t)x * x >> 16) * 3 + x) >> 2);
}

/* The target's -Os emits actual divides even for /16 and /6. Preserve C's
 * truncation towards zero with shifts / reciprocal multiplication instead. */
static inline __attribute__((always_inline)) int32_t super_div16(int32_t x)
{
    return x < 0 ? -((-x) >> 4) : x >> 4;
}
static inline __attribute__((always_inline)) int32_t super_div6(int32_t x)
{
    return (int32_t)(((int64_t)x * 715827883) >> 32) + (x < 0);
}

/* floor(UINT32_MAX/d) is a low reciprocal of 2^32/d, with error <=1.
 * Thus high32(n*r) is either n/d or one below it for every uint32 n.
 * A remainder check recovers the EXACT quotient, without a sample-rate divide.
 * d=0 is never used for a wrap (no correction); cache zero in that case. */
static inline __attribute__((always_inline)) uint32_t super_reciprocal(uint32_t d)
{
    return d ? 0xFFFFFFFFu / d : 0;
}
static inline __attribute__((always_inline)) uint32_t super_fraction(uint32_t n, uint32_t d, uint32_t r)
{
    uint32_t q = (uint32_t)(((uint64_t)n * r) >> 32);
    return q + (n - q * d >= d);
}

/* Specialize osc_step for a natural saw wrap. Phase and delayed sample stay
 * in registers for the whole block; H=-65536 makes the event gains exact
 * +/-1, so there are no extra multiplies/shifts. Bit-identical to osc_step. */
static inline __attribute__((always_inline)) void super_saw_run(osc_t *o, uint32_t inc,
                                                               uint32_t reciprocal, int32_t *out, uint32_t n, int add)
{
    uint32_t ph = o->ph, d = inc >> 15;
    int32_t prev = o->prev;
    for (uint32_t i = 0; i < n; i++) {
        uint32_t old = ph; ph += inc;
        int32_t a = prev, cur = (int32_t)(ph >> 16) - 32768;
        if (ph < old && d) {
            int32_t t = (int32_t)super_fraction(ph,d,reciprocal), u = 32767 - t;
            a -= (t * t) >> 15;
            cur += (u * u) >> 15;
        }
        if (add) out[i] += a; else out[i] = a;
        prev = cur;
    }
    o->ph = ph; o->prev = prev;
}

/* Two independent saws per pass: same delayed/wrap equations and exact sum,
 * three buffer passes instead of zeroing plus six read/modify/write passes. */
static inline __attribute__((always_inline)) void super_saw_pair(osc_t *a,osc_t *b,
 uint32_t ia,uint32_t ib,uint32_t ra,uint32_t rb,int32_t*out,uint32_t n,int add){
 uint32_t pa=a->ph,pb=b->ph,da=ia>>15,db=ib>>15;
 int32_t va=a->prev,vb=b->prev;
 for(uint32_t i=0;i<n;i++){
  uint32_t oa=pa,ob=pb;pa+=ia;pb+=ib;
  int32_t y=va+vb,ca=(int32_t)(pa>>16)-32768,cb=(int32_t)(pb>>16)-32768;
  if(pa<oa&&da){int32_t t=(int32_t)super_fraction(pa,da,ra),u=32767-t;y-=(t*t)>>15;ca+=(u*u)>>15;}
  if(pb<ob&&db){int32_t t=(int32_t)super_fraction(pb,db,rb),u=32767-t;y-=(t*t)>>15;cb+=(u*u)>>15;}
  if(add)out[i]+=y;else out[i]=y;va=ca;vb=cb;
 }
 a->ph=pa;a->prev=va;b->ph=pb;b->prev=vb;
}

/* An inaudible oscillator still advances to exactly the same state as a full
 * render. Only the last naive sample and its wrap correction affect prev. */
static inline __attribute__((always_inline)) void super_saw_skip(osc_t *o, uint32_t inc, uint32_t reciprocal, uint32_t n)
{
    uint32_t ph = o->ph + inc * n, d = inc >> 15;
    int32_t cur = (int32_t)(ph >> 16) - 32768;
    if (ph < inc && d) {
        int32_t u = 32767 - (int32_t)super_fraction(ph,d,reciprocal);
        cur += (u * u) >> 15;
    }
    o->ph = ph; o->prev = cur;
}

#ifdef OBV_HOST
#define SUPER_HOT
#else
/* Keep the hot loop out of the flash instruction cache. No out-of-RAM calls;
 * tools/build.py audits this together with the existing RAM routines. */
#define SUPER_HOT __attribute__((section(".ram_text.super"), noinline))
#endif
#ifdef OBV_SUPER_KERNEL
#define SUPER_LINKAGE
#else
#define SUPER_LINKAGE static
#endif
SUPER_LINKAGE SUPER_HOT void super_render(super_state_t *s, osc_t *center, uint32_t inc,
                         uint32_t det, uint32_t mix, int32_t *out, uint32_t n)
{
    /* Mutable storage deliberately lives in RAM even on the target. */
    static int32_t off[6] __attribute__((section(".data.super"))) = {-7210, -4121, -1279, 1305, 4074, 7042};
    uint32_t j, i;
    if (!s->active) {
        uint32_t seed = center->ph ^ 0x9E3779B9u;
        for (j = 0; j < 6; j++) {
            seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
            s->side[j].ph = seed;
            s->side[j].prev = (int32_t)(seed >> 16) - 32768;
        }
        s->det_sm = (int32_t)det * 256;
        s->mix_sm = (int32_t)mix * 256;
        s->active = 1;
        s->inc_cached = 0; s->det_cached = 128; s->mix_cached = -1;
    }
    s->det_sm += super_div16((int32_t)det * 256 - s->det_sm);
    s->mix_sm += super_div16((int32_t)mix * 256 - s->mix_sm);
    uint32_t shape = (uint32_t)s->det_sm >> 8;
    /* Keep every side below Nyquist even at extreme MIDI pitches. */
    if (inc > 0x70000000u) inc = 0x70000000u;
    uint32_t changed = shape != s->det_cached;
    if(changed){
        uint32_t d=super_curve(shape);
        for(j=0;j<6;j++)s->det_offset[j]=off[j]*(int32_t)d;
        s->det_cached=shape;
    }
    if (inc != s->inc_cached || changed) {
        for (j = 0; j < 6; j++) {
            uint32_t next = (uint32_t)((int64_t)inc + (((int64_t)inc * s->det_offset[j]) >> 32));
            if ((next >> 15) != (s->si[j] >> 15)) s->reciprocal[j] = super_reciprocal(next >> 15);
            s->si[j] = next;
        }
        if ((inc >> 15) != (s->inc_cached >> 15)) s->reciprocal[6] = super_reciprocal(inc >> 15);
        s->inc_cached = inc;
    }
    /* Central + six sides at up to half its level, peak normalized.
     * Compute the side average first: every multiply fits in int32. */
    uint32_t m = (uint32_t)s->mix_sm;
    if ((int32_t)m != s->mix_cached) {
        s->cg = (int32_t)((32768u * 32512u) / (32512u + 3u * m));
        s->mix_cached = (int32_t)m;
    }
    int32_t cg = s->cg;
    int32_t sg = 32768 - cg;
    if (!out) {
        super_saw_skip(center, inc, s->reciprocal[6], n);
        for (j = 0; j < 6; j++) super_saw_skip(&s->side[j], s->si[j], s->reciprocal[j], n);
        return;
    }
    super_saw_run(center, inc, s->reciprocal[6], out, n, 0);
    if (!sg) {
        for (j = 0; j < 6; j++) super_saw_skip(&s->side[j], s->si[j], s->reciprocal[j], n);
        return;
    }
    int32_t side[CTL];
    for (j = 0; j < 6; j+=2) super_saw_pair(&s->side[j],&s->side[j+1],s->si[j],s->si[j+1],s->reciprocal[j],s->reciprocal[j+1],side,n,j!=0);
    for (i = 0; i < n; i++) out[i] = (out[i] * cg + super_div6(side[i]) * sg) >> 15;
}

/* Companion SAW in a SUPER voice: the same exact natural-wrap kernel, with
 * no sync/XMOD (already bypassed for this voice). Legacy voices are untouched. */
SUPER_LINKAGE SUPER_HOT void super_plain_saw(super_plain_state_t *s, osc_t *o, uint32_t inc,
                                            int32_t *out, uint32_t n)
{
    if (s->inc_cached != inc) {
        if ((inc >> 15) != (s->inc_cached >> 15)) s->reciprocal = super_reciprocal(inc >> 15);
        s->inc_cached = inc;
    }
    if (out) super_saw_run(o, inc, s->reciprocal, out, n, 0);
    else super_saw_skip(o, inc, s->reciprocal, n);
}
#undef SUPER_HOT
#undef SUPER_LINKAGE
#else
void super_render(super_state_t *s, osc_t *center, uint32_t inc,
                  uint32_t det, uint32_t mix, int32_t *out, uint32_t n);
void super_plain_saw(super_plain_state_t *s, osc_t *o, uint32_t inc, int32_t *out, uint32_t n);
#endif

static inline void super_render_call(super_state_t *s, osc_t *center, uint32_t inc,
                                     uint32_t det, uint32_t mix, int32_t *out, uint32_t n)
{
#ifdef OBV_HOST
    super_render(s, center, inc, det, mix, out, n);
#else
    /* Same far-call mechanism as the existing flash RAM routines: XIP->RAM
     * exceeds the target's relative call range. No interrupt masking. */
    __typeof__(&super_render) volatile fn = &super_render;
    fn(s, center, inc, det, mix, out, n);
#endif
}

static inline void super_plain_call(super_plain_state_t *s, osc_t *o, uint32_t inc, int32_t *out, uint32_t n)
{
#ifdef OBV_HOST
    super_plain_saw(s,o,inc,out,n);
#else
    __typeof__(&super_plain_saw) volatile fn = &super_plain_saw;
    fn(s,o,inc,out,n);
#endif
}
