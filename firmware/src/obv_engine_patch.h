/* SPDX-License-Identifier: GPL-3.0-only */
/* Typed patches in the six reserved trailing bytes. Legacy bytes/names/CCs keep
 * their offsets. No automatic writes or conversion of legacy patches. */
enum { OBV_ANALOG, OBV_VOICE, OBV_DRUMS, OBV_ENGINE_COUNT };
#define OBV_ENGINE_TAG_AT 74u
_Static_assert(NPARAM + PATCH_NAME <= OBV_ENGINE_TAG_AT, "engine tag overlaps patch data");
_Static_assert(OBV_ENGINE_TAG_AT + 6u == PATCH_BYTES, "engine tag layout");
static uint32_t obv_patch_engine(const patch_t *p)
{
    const uint8_t *t = p->v + OBV_ENGINE_TAG_AT;
    if (t[0] != 'O' || t[1] != 'V' || t[2] != 'E' || (t[3] != 1 && !(t[3] == 2 && t[4] == OBV_VOICE)) ||
        t[4] >= OBV_ENGINE_COUNT || t[5] != (uint8_t)(t[4] ^ 0xA5u)) return OBV_ANALOG;
    return t[4];
}
static void obv_patch_set_engine(patch_t *p, uint32_t id)
{
    uint8_t *t = p->v + OBV_ENGINE_TAG_AT;
    t[0]='O'; t[1]='V'; t[2]='E'; t[3]=1; t[4]=(uint8_t)id; t[5]=(uint8_t)(id ^ 0xA5u);
}
/* Retired engine id 1 is never reused: old DRUMS tags remain id 2. */
static int obv_patch_sanitize(patch_t*p){
 if(obv_patch_engine(p)!=OBV_VOICE)return 0;
 uint8_t fx[P_REV_MIX-P_CHO_RATE+1];
 for(unsigned i=0;i<sizeof fx;i++)fx[i]=p->v[P_CHO_RATE+i];
 patch_defaults(p);
 for(unsigned i=0;i<sizeof fx;i++)p->v[P_CHO_RATE+i]=fx[i];
 const char*name="INIT ANA";for(unsigned i=0;i<PATCH_NAME&&name[i];i++)p->v[NPARAM+i]=(uint8_t)name[i];
 obv_patch_set_engine(p,OBV_ANALOG);return 1;
}
static uint32_t obv_param_max(const patch_t*p,uint32_t id){return id==P_O2_WAVE&&obv_patch_engine(p)==OBV_ANALOG?W_BOTH:PDEF[id].max;}

/* KBO1: one reserved byte, before the engine tag; absent legacy tag = octave0. */
#define PATCH_KBO_AT (OBV_ENGINE_TAG_AT-1u)
_Static_assert(NPARAM+PATCH_NAME<=PATCH_KBO_AT,"keyboard octave overlaps patch name");
static int patch_keyboard_octave(const patch_t*p){unsigned v=p->v[PATCH_KBO_AT];return v>=0xD0u&&v<=0xD4u?(int)v-0xD2:0;}
static void patch_keyboard_octave_set(patch_t*p,int oct){p->v[PATCH_KBO_AT]=(uint8_t)(0xD2+(oct < -2 ? -2 : oct > 2 ? 2 : oct));}

/* SS1 extension occupies only the ANALOG engine's six trailing bytes.
 * NPARAM, name, octave, PATCH_BYTES and every MIDI CC retain their offsets.
 * Unknown/absent extension -> MIX 64. DRUM-X keeps its OVE tag unchanged.
 * Readers never rewrite legacy bytes. Checksum is committed last by setter. */
static int patch_super_valid(const patch_t *p)
{
    const uint8_t *t = p->v + OBV_ENGINE_TAG_AT;
    return t[0]=='S' && t[1]=='S' && t[2]==1 && t[3]<=127 && t[4]<=127 &&
        t[5]==(uint8_t)(0xA5u ^ t[3] ^ t[4]);
}
static uint32_t patch_super_mix(const patch_t *p, uint32_t osc)
{
    return patch_super_valid(p) ? p->v[OBV_ENGINE_TAG_AT+3u+(osc&1u)] : 64u;
}
static void patch_super_mix_set(patch_t *p, uint32_t osc, uint32_t value)
{
    uint8_t a=(uint8_t)patch_super_mix(p,0), b=(uint8_t)patch_super_mix(p,1);
    uint8_t *t=p->v+OBV_ENGINE_TAG_AT;
    if(obv_patch_engine(p)!=OBV_ANALOG)return;
    value=value>127u?127u:value;
    if(osc&1u)b=(uint8_t)value;else a=(uint8_t)value;
#ifndef OBV_HOST
    uint32_t irq=fm1__lock();
#endif
    t[0]='S';t[1]='S';t[2]=1;t[3]=a;t[4]=b;t[5]=(uint8_t)(0xA5u^a^b);
#ifndef OBV_HOST
    fm1__unlock(irq);
#endif
}
