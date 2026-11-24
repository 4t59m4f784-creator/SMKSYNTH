# SMKSTAGE B3 0.1.3-D4 — CPU0-only Tonewheel Experiment

**Not validated on hardware. Research-only variant. Never replace the D3 or stable 0.1.1.**

Based exactly on the D3 IRQ-isolation branch, with one deliberately small architectural change in `firmware/src/smkstage.c`:

- `ORGAN_C1_RUN(fn,arg)` resolves to 0, so `organ_split` uses its existing `wheels_render(0,1,n,acc,accv,...)` fallback, rendering all of the same 91 tonewheels on CPU0.
- `ORGAN_C1_WAIT` is bypassed and no tonewheel work is dispatched to CPU1. The second core may still be initialized but remains idle from the organ engine's standpoint.
- All original setBfree tonewheel calculations, Leslie, chorus, percussion, Dattorro plate, drawbar mapping, F1 direction, MOD wheel, factory presets/library, two-sector flash persistence, UI and crash reporting remain the same.
- Hardware TIMER5 IRQ remains in RAM as in D3. No change to timer frequency (10 kHz), scanner priority (4), ALNK0 audio priority (3), USB audio or bootloader.

## Hypothesis and interpretation

Repeated instrument failures under heavy drawbars / chords / Leslie / chorus (with and without reverb) have the same PC-limit crash:
- VEC `00000001`
- PC `030004A8` in the earlier XIP-wrapper builds; the actual D3 PC/T63 should be reported separately because TIMER5 moved into RAM
- DBG `00001000`, violating executable PC limit; T63 originally `020004A6`
- RETS within audio interrupt, near CPU1 work scheduling.

This test isolates CPU1 tonewheel job dispatch from the audio path. Under high load, D4 will likely miss audio deadlines and produce audible glitches *more readily* than D3 because all 91 wheels now run sequentially. **An audio glitch or overload alone is not evidence that the crash is fixed.**

Interpretation:
- If D4 crackles but never PC-limit crashes at the same musical load while D3 does, CPU1 handoff and/or concurrent resource pressure becomes a stronger suspect, not proof.
- If D4 has the same crash, CPU1 handoff is not necessary to trigger it; focus on ISR entry/fetch and bus/interrupt protection.
- If D4 is unplayable due to CPU0 overload, reduce chords gradually to find a matched operating range; the experiment may be inconclusive.

## Conservative instrument test

1. Keep a copy of physically verified 0.1.1 `.fwsc`, the original factory V016 and existing fallback installer, in the same USB-MIDI Chrome profile.
2. Only install on known SMK-37 Pro. Run boot/quiet audio first, then gradually increase drawbars, chord polyphony and Leslie speed/chorus; leave reverb at original test setting.
3. Do **not** use a stress pattern likely to cause sustained audio underruns. If crackling is persistent, stop the load test and report the threshold instead.
4. Photograph all seven crash fields (VEC/PC/RETS/DBG/EMU/T63/SP) if failure happens, and compare with D3.
5. If Rescue `SMK-37 Pro_000`, do not reinstall D4. Recover to 0.1.1 using the pinned fallback installer in the same browser. Do not erase arbitrary flash.

The exact .fwsc SHA-256 is recorded by the successful GitHub Actions build and the separate, pinned D4 installer; keep 0.1.1 and D3 unchanged.
