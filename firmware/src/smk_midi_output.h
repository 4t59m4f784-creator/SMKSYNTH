/* SPDX-License-Identifier: GPL-3.0-only */
static int midi_send_ready(void) {
#if BOARD_SMK37
 /* USB may enumerate for power/audio without consuming MIDI IN. Its queue
  * must never stop the independently clocked cable. USB overflow recovers
  * at its consumer with all-channel sustain/note/sound off. */
 return smk_tx_ready();
#else
 return usb.config && mo_w-mo_r<MQ;
#endif
}
static void midi_send(uint32_t st, uint32_t d1, uint32_t d2)   /* from the main loop only */
{
#if BOARD_SMK37
    if(!midi_send_ready())return;
    smk_tx_message(st,d1,d2);
#endif
    midi_out_event((st >> 4) | st << 8 | (d1 & 0x7Fu) << 16 | (d2 & 0x7Fu) << 24);
}

