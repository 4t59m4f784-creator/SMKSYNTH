/* SPDX-License-Identifier: GPL-3.0-only */
/* Physical switch pedal. Connect before boot, released: its initial level is
 * the rest polarity. Hysteresis + time debounce use the real mux ADC reading.
 * Physical and incoming MIDI sustain have independent ownership. */
static struct {
    uint32_t since, ready, rest_high, candidate, down, blocked;
} smk_pedal;
static uint32_t smk_cc_pending, smk_panic_tx;
static uint8_t smk_cc_value[16];
static void smk_cc_schedule(unsigned mask,unsigned value)
{
    for(unsigned ch=0;ch<16;ch++)if(mask>>ch&1u)smk_cc_value[ch]=(uint8_t)value;
    smk_cc_pending|=mask;
}
static uint32_t smk_pedal_channels;
/* Current four parts plus actual still-owned notes on an old base. This is
 * deliberately not the historical "ever used" mask. Called from main only. */
static unsigned smk_sustain_channels(void)
{
    unsigned mask=15u<<mout_base;
    fm1_irq_off();
    for(unsigned ch=0;ch<16;ch++)for(unsigned n=0;n<128;n++)
        if(mout_refs[ch][n]){mask|=1u<<ch;break;}
    fm1_irq_on();return mask;
}

static void sustain_apply(void) { ctl_sustain = midi_sustain || smk_pedal.down; }

/* One packet per poll, only when the USB ring has room. A panic cannot lose
 * its CCs to a full queue. Panic covers all 16 channels; sustain covers the
 * four part channels and channels still owning their previous notes. */
static void smk_midi_controls_poll(void)
{
    unsigned base=set.midi_ch?set.midi_ch-1u:0u;
    if(base>12u)base=12u;
    if(base!=mout_base){mout_base=base;if(smk_pedal.down){unsigned mask=smk_sustain_channels();smk_pedal_channels|=mask;smk_cc_schedule(mask,127);}}
    if(mout_fault){fm1_irq_off();mout_clear();fm1_irq_on();smk_panic_tx=48;}
    if (!midi_send_ready()) return;
    if (smk_panic_tx) {
        unsigned i = 48u - smk_panic_tx;
        static const uint8_t cc[3] = {64, 123, 120};
        midi_send(0xB0u | (i / 3u), cc[i % 3u], 0);
        smk_panic_tx--;
        if(!smk_panic_tx)mout_blocked=0;
    } else if (smk_cc_pending) {
        unsigned ch = 0;
        while (!(smk_cc_pending >> ch & 1u)) ch++;
        midi_send(0xB0u | ch, 64, smk_cc_value[ch]);
        smk_cc_pending &= ~(1u << ch);
    }
    if(!smk_panic_tx&&!mout_blocked)mout_poll();
}

static void smk_sustain_poll(void)
{
    unsigned raw = fm1_in.pedal, high;
    if (fm1_in.frames < 16u) return; /* wait for completed ADC scans */
    if (!smk_pedal.ready) {
        high = raw >= 512u;
        if (!smk_pedal.since || high != smk_pedal.rest_high) {
            smk_pedal.rest_high = high; smk_pedal.since = fm1_ms + 1u;
        }
        if (fm1_ms + 1u - smk_pedal.since >= 100u) smk_pedal.ready = 1;
        return;
    }
    high = smk_pedal.rest_high ? raw < 256u : raw > 768u;
    if (smk_pedal.down) high = smk_pedal.rest_high ? raw < 768u : raw > 256u;
    if (high != smk_pedal.candidate) { smk_pedal.candidate = high; smk_pedal.since = fm1_ms; }
    if (fm1_ms - smk_pedal.since < 12u) return;
    if (smk_pedal.blocked) {
        if (!high) smk_pedal.blocked = 0;
        return; /* panic: must release before sustain can hold notes again */
    }
    if (high == smk_pedal.down) return;
    smk_pedal.down = high; sustain_apply();
    unsigned mask=smk_sustain_channels();
    if(high){smk_pedal_channels|=mask;smk_cc_schedule(mask,127);}
    else {smk_cc_schedule(mask|smk_pedal_channels,0);smk_pedal_channels=0;}
}

static void smk_panel_panic(void)
{
    fm1_irq_off();
    ctl_seq_run = ctl_seq_restart = ctl_count = 0;
    rec = REC_OFF; rec_down = rec_wait = 0;
    hold = ctl_hold = midi_sustain = ctl_sustain = 0;
    smk_pedal.blocked = smk_pedal.down || smk_pedal.candidate;
    smk_pedal.down = 0;
    memset(kb_press, 0, sizeof kb_press); memset(kb_refs, 0, sizeof kb_refs);
    memset(panel_live_note, 0, sizeof panel_live_note);
    memset(transpose_captured, 0, sizeof transpose_captured);
    for (unsigned t=0;t<NTRK;t++) for (unsigned w=0;w<4;w++) ctl_live_hold[t][w]=0;
    memset(rec_open_part, 0, sizeof rec_open_part);
    memset(smk_live, 0, sizeof smk_live); memset(drum_pad_down, 0, sizeof drum_pad_down);
    memset(pf_role, 0, sizeof pf_role); smk_pads_held = 0;
    memset(audition, 0, sizeof audition); memset(erase_on, 0, sizeof erase_on);
    notes_down = step_midi = 0; step_held = -1; entry_reset();
    arp_qr = arp_qw;
    mout_blocked=1;mout_clear();
    ctl_panic = 2; /* ISR discards queued notes and immediately kills voices */
    fm1_irq_on();
    smk_cc_pending = smk_pedal_channels = 0; smk_panic_tx = 48;
    dirty_hdr = dirty_page = dirty_steps = dirty_tags = 1;
    say("MIDI PANIC");
}

/* GLOBE + PLAY, either order; swallow both releases to avoid page/transport
 * changes. No hold timer: it must work promptly during a stuck-note event. */
static uint32_t smk_panic_buttons(uint32_t b, uint32_t *rel)
{
    static uint32_t blocked;
    const uint32_t pair = (1u << BTN_GLO) | (1u << BTN_PLAY);
    if (!blocked && (fm1_in.buttons & pair) == pair && (b & pair)) {
        blocked = 1; smk_panel_panic(); layer = L_NONE; layer_lock = 0;
    }
    if (blocked) {
        b &= ~pair; *rel &= ~pair;
        if (!(fm1_in.buttons & pair)) blocked = 0;
    }
    return b;
}
