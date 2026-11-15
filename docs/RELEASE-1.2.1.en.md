# SMKSYNTH 1.2.1 — Endless MIDI

Public release based on hardware-approved Endless MIDI A /19652. One installer automatically identifies Pro or Elite and selects the matching package.

- **ENDLESS:** four-part note/chord sequences with independent lengths and per-part USB/TRS MIDI output. PARA enables/disables without clearing; REC records; holding REC clears the selected part. C4 retains original pitch. Opening the page does not start playback.
- **MIDI MODE:** independent USB and TRS/DIN controller. Bank A retains configurable pads; bank B selects keyboard channels 1–16. One pad alone selects one channel. Hold a pad while pressing others to create a layer; release all pads to latch it. A new single-pad gesture replaces the layer.
- **Wheels and sustain:** keyboard, pitch bend, mod wheel and pedal follow all selected channels. Encoders, faders and bank A pads keep their map channels. Note Off and pedal-off reach original destinations after selection changes. Bank B selection is temporary and resets to the map main channel on mode entry or map change.
- **Battery and version:** four approximate bands in both themes; red low-battery indication and grey unavailable reading. Endless MIDI edition name and technical version appear on the splash and GLOBE > THEME.
- **TRS independent of USB:** a connected USB host that does not consume MIDI can no longer block the cable. If USB exceeds its queue capacity, recovery clears stale notes before accepting new gestures.

Retained: optional Zoom theme (Obervave default), dedicated oscilloscope, preset renaming, automatic sustain polarity and MIDI PANIC with GLOBE + PLAY. **Supersaw remains exclusive to OSC 1 to preserve stability**, with a five-voice limit when used. No Bluetooth MIDI or outgoing MIDI Clock/Start/Stop in this release.

**Updating:** preserves saved bank, programs, patterns, favourites, theme and MIDI maps. Direct updates from ML1, ML1a and Endless MIDI A are supported. First installation requires the connected model’s official V016 and prepares the factory bank. Export a backup first; MIDI maps and theme are not in the .smksynth file. Identities: Pro_19653 / Elite_19653.

[Installer](https://4t59m4f784-creator.github.io/SMKSYNTH/install/?lang=en) · [Manual](MANUAL.en.md) · [Quick start](QUICKSTART.en.md) · [Português](RELEASE-1.2.1.md)
