/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static psnd_t saved[2];
static pattern_t steps[2];
static void edit(unsigned seed) {
    for (unsigned t=0;t<NTRK;t++) {
        trk_load(t,(seed+t)%NPROG);
        live_p[t].v[P_ARP_MODE]=(seed+t)%2;
        live_p[t].v[P_CUTOFF]=20+seed+t;
        trk_edited[t]=1;
        set.mix_lvl[t]=50+seed+t; set.mix_pan[t]=(int8_t)(seed+t);
        set.mix_dly[t]=seed+t; set.mix_rev[t]=80-seed-t;
        set.mix_cho[t]=30+seed+t; set.mix_drv[t]=seed;
    }
    for(unsigned i=0;i<NFXP;i++)set.fx[i]=20+seed+i;
    set.fx_ok=FX_OK;fx_keep(&live_p[0]);
    set.mix_mute=seed&3;set.mix_solo=0;
    set.dv_type=0;set.dv_amt=seed;set.dv_tone=45;set.dv_mix=seed;
    set.m_vol=100+seed;set.m_dj=64;set.m_drive=seed;set.m_comp=seed;
    pat_init(&pat);pat.st[0][0].note[0]=60+seed;
    pat.lane[0][0].param=P_CUTOFF;pat.lane[0][0].v[0]=seed;
    mix_apply();pat_edited=1;
}
static void verify(unsigned n) {
    mixset_t m={0};mx_from_set(&m);
    assert(!memcmp(&m,&saved[n].mx,sizeof m));
    for(unsigned t=0;t<NTRK;t++) {
        for(unsigned i=0;i<sizeof(patch_t);i++) if(((uint8_t*)&live_p[t])[i]!=((uint8_t*)&saved[n].p[t])[i]) {fprintf(stderr,"scene %u part %u byte %u got %u wanted %u run %u\n",n,t,i,((uint8_t*)&live_p[t])[i],((uint8_t*)&saved[n].p[t])[i],ctl_seq_run);assert(0);}
        assert(trk_prog[t]==saved[n].prog[t]);
        assert(trk_edited[t]==saved[n].edited[t]);
    }
    assert(!memcmp(&pat,&steps[n],sizeof pat));
    assert(!(snd_dirty>>n&1));assert(!pat_edited);
    assert(smk_master==333);
}
int main(void) {
    engine_init();memset(&set,0,sizeof set);step_held=-1;
    for(unsigned n=0;n<2;n++) {
        set.pat=n;edit(n+1);snd_capture(n);
        saved[n]=psnd[n];steps[n]=pat;
        assert(!slot_save(n,&pat,sizeof pat,&psnd[n],sizeof psnd[n]));
        slot_saved(n,&pat);
    }
    smk_master=333;unsigned writes=host_write_count[2];
    /* Reproduce A/B edits and cache capture without ever overwriting flash. */
    for(unsigned k=0;k<40;k++) {
        unsigned n=k%2;edit(10+k%10);snd_capture(set.pat);
        psnd[n].p[0].v[P_ARP_MODE]^=1;psnd[n].mx.fx[0]^=1;
        ctl_seq_run=0;pat_go(n);verify(n);
        /* Recalling the selected slot discards its unsaved edits too. */
        edit(12);snd_capture(n);pat_go(n);verify(n);
    }
    /* Playing: the queued sounds/mix must be from flash, including same-slot recall. */
    for(unsigned n=0;n<2;n++)for(unsigned same=0;same<2;same++) {
        ctl_seq_run=0;pat_go(n);edit(11);snd_capture(n);
        ctl_seq_run=1;seq_was=0;pat_go(same?n:1-n);
        unsigned target=same?n:1-n;
        assert(pat_queue==target+1);assert(!ctl_pat_keep);pat_old.magic=0x13579BDFu;assert(!memcmp(snd_q,saved[target].p,sizeof snd_q));
        assert(!memcmp(&pat_q,&steps[target],sizeof pat_q));
        /* Step automation legitimately changes the patch at the first tick. */
        pat_q.lane[0][0].param=AUTO_NONE;
        seq_tick();assert(pat_old.magic==0x13579BDFu);assert(!ctl_pat_swap);assert(ui_snd_swapped);
        pat.lane[0][0]=steps[target].lane[0][0];
        set.pat=target;snd_meta(target);pat_queue=0;pat_edited=0;
        ui_snd_swapped=0;verify(target);
    }
    assert(host_write_count[2]==writes);
    puts("PASS: 80 stopped recalls, 4 queued audio-ISR recalls, full patches/ARP/FX/mixer/steps/automation, K1 retained, zero recall writes.");
    return 0;
}
