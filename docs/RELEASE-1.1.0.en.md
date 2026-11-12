# SMKSYNTH 1.1.0 - MIDI, sustain and Pro / Elite


1.1.0 promotes MIDI M1.2, hardware-validated by Marcus on SMK-37 Pro, and combines Pro and Elite in one installer. The Elite installation path was also validated by a real user. Automatic identification selects a model-specific package; no manual firmware selection is required.

- USB and TRS/DIN output transmit all four parts: keys, pads, Split/Layer, ARP and sequencers. A uses GLOBAL MIDI CH base 1-13; B-D use the next channels. OMNI transmits 1-4. DRUM-X uses its part channel. Note Off retains the original channel and pitch.
- Physical Pitch Bend and CC1 follow the selected part, including Split/Layer. Old channels are neutralized on focus changes; wheel state is resent after Panic/queue recovery.
- Switch-pedal sustain detects polarity at startup. Connect before power-on, released. No half-pedal support. Sustain covers all melodic parts and releases previous channels correctly; pending CC64 values are independent per channel.
- GLOBE + PLAY stops transport/REC, clears HOLD/sustain and pending notes, kills voices and sends CC64/123/120 off on all 16 channels. Release the pedal before reusing sustain. Effects tails may finish.
- Fixed old POLY/DUAL voices remaining gated after a live change to MONO/UNISON. Glide is unchanged; Glide-only edits did not reproduce the bug in tests.
- One installer identifies Pro or Elite and chooses first installation from model-specific stock V016, or a bank-preserving SMKSYNTH update. Export a backup first. Old interrupted installers are preserved.

Target identities are `SMK-37 Pro_19634` and `SMK-37 Elite_19634`. Factory patterns/favourites are retained. ENDLESS remains separate. Sequence notes transmit; MIDI Clock/Start/Stop output and automatic preset Program Changes are not included. Incoming bend remains global on the base channel. USB audio remains supported; iPhone USB video clicks/dropouts remain a known issue. Original credits and GPL-3.0-only notices are preserved.
