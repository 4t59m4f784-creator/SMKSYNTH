/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
#include <math.h>

/* Exercise the real oscillator pitch calculation, not just the controller
 * variables: Vintage uses the per-voice PRNG even with both wheels at rest. */
static void check_tuning(const char *phase)
{
    static const unsigned vintage[4] = {0, 32, 80, 127};
    for(unsigned t=0;t<4;t++){
        live_p[t].v[P_VINTAGE]=vintage[t];
        block_globals(&pt[t]);
        voice_start(&vc[t],t,60,100,0,0);
    }
    for(unsigned b=0;b<4000;b++)for(unsigned t=0;t<4;t++)voice_block(&vc[t],t);
    unsigned bad=0;
    for(unsigned t=0;t<4;t++){
        double cents=1200.0*log2((double)vc[t].inc1/pitch_inc(60*256));
        printf("%s part %u Vintage %u: %.2f cents, RNG %08x\n",phase,t,vintage[t],cents,vc[t].noise);
        fflush(stdout);
        bad |= fabs(cents)>=100.0 || !vc[t].noise; /* a semitone is never normal drift */
    }
    assert(!bad);
}
int main(void)
{
    engine_init();pf_init();pat_init(&pat);mm_boot();
    fm1_ms=1000;fm1_in.frames=400;fm1_in.wheel[0]=535;fm1_in.wheel[1]=620;
    for(unsigned t=0;t<4;t++){
        patch_from_factory(&live_p[t],&FACT_INIT);
        live_p[t].v[P_GLIDE]=live_p[t].v[P_LFO_PITCH]=live_p[t].v[P_MOD_DEPTH]=0;
        live_p[t].v[P_ENV_O2]=64;
    }
    smk_wheels();check_tuning("boot");
    patch_t before[4];memcpy(before,live_p,sizeof before);
    for(unsigned n=0;n<10;n++){
        mm_enter();mm_leave();smk_wheels();
        assert(!memcmp(before,live_p,sizeof before));
        check_tuning("MIDI return");
        /* GLOBE+PLAY uses the same hard reset, without a mode switch. */
        smk_panel_panic();engine_events();
        assert(!memcmp(before,live_p,sizeof before));
        check_tuning("panic");
    }
    puts("PASS: ten MIDI returns and ten PANIC resets preserve four patches and tuning with Vintage 0/32/80/127.");
}
