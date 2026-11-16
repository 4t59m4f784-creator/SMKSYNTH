# SMKSYNTH 1.2.2-dc2a / 19658

Experimental preserving update for Pro and Elite. Official release 1.2.2 and its firmware/simulator are unchanged. Based on DC1 commit `42e63ff9f04ef256b33db2bb0aaf5ab7587cdc59`.

## Changes

- Dedicated snapshot copies use aligned, unrolled 32-bit may_alias accesses with safe byte fallback and tail handling. Padded private parameter rows are aligned; only the original 63 parameters are passed to the unchanged drum DSP. State remains private until successful CPU1 completion; the original fallback remains.
- The busiest active DRUM-X kit still runs on CPU1. Synth voices, remaining kits, mix and effects stay on CPU0. No DSP equations, presets, sample/control rate, effect quality, voice ceilings, audio buffer sizes or clock changes.
- GLOBE + SCALE opens the diagnostic overlay. K1 selects the next 1C/2C mode, K2 arms/resets only while transport is stopped, PATCH exits. PLAY starts the 30-second wall-clock capture. Results freeze automatically; stopping early freezes a shorter window. K3 cycles three frozen result pages. FROZEN reports the captured mode; NEXT reports the selection for the next capture. Navigation before PLAY and after freezing is excluded.
- Per-half CPU0 measurements: control/events/sequence, normal synth voices, SUPER voices, CPU0 drums, snapshot copies/commit, CPU1 wait, effects/drive/master, mix, residual IO/other and nested scan/USB handler. CPU0 stage times subtract measured nested TIMER5 body time. Assembly IRQ entry/exit overhead and profiler bookkeeping fall in residual IO/other. CPU1 worker peak remains a per-control-block wall time, not a per-half CPU0 metric.
- Counts are recorded only during the capture. Profiling adds timing reads/bookkeeping during capture; it can affect the measured headroom. Per-stage peaks are not necessarily simultaneous. The IRQ OVER counter does not detect all late-start/missed-DMA conditions or all audible glitches. CLIP SAMPLES detects final output clamping, not all upstream saturation.
- Backup recognizes identities 19656 (DC1) and 19658 (DC2a) for the unchanged SMK16P1/B2R1 format. No user's bank, samples, MIDI content or musical records are distributed.

## Validation

Host threaded PCM/state equivalence with original DC1 test session, dispatch rejections, corrupted private results/fallback and live mode changes: unchanged SHA-256 `30a0109ffec4af806d0b6cab29d279864e538acdafe8011dcc531e9482a9659e`. ASan/TSan passed.

The supplied private pattern 9 was rendered locally for 174 seconds through DC1 and DC2a serial/worker paths: identical PCM plus final drum state (`7444ece921c6acbd453977a2e32d9e0676e38eb942eb135ae2f223aba5c17b9b`). Peak occupancy: 8 melodic voices, 5 SUPER, 4 drum hits. These are host checks, not proof of real-time chip headroom. User records/audio are not checked in; the generic `tests/pattern_bank_audio_test.c` accepts external records.

Standalone window tests cover transport-start arming, freezing, early stop, timer wrap, reset, nested time subtraction and residual accounting. The host UI test covers GLOBE+SCALE, K1 selection, K2 arm, PATCH exit, PLAY start, freeze, page browse and stopped-only rearm. Copy bounds/alignment/tails pass ASan+UBSan. Existing physical-controls/MIDI, Vintage/PANIC tuning and drum tests are retained.

Pro/Elite target image, RAM, pool, stack and MMIO checks pass. Four model-specific installer/return packages retain CRC/hash checks and preserving update layout. Actual installation, sound, stability, benchmark and controls on a physical SMK-37 remain unverified. No numerical speedup or glitch fix is claimed yet.

## Build

Use the existing pinned toolchain/SDK instructions. Build without `--first-install`:

```sh
OBV_DEFS=-DOBV_DUAL_CORE=1 JIELI_TOOLCHAIN=/path/to/toolchain \
python3 tools/build.py --voice-hardware --board smk37 --model pro \
 --release 1.2.2-dc2a --experimental-id 19658 --sdk /path/to/AC79-SDK
```

Repeat with `--model elite`. Do not replace the frozen DC1 installer assets; pending installations must keep their original package pins.

## Coherent timestamp correction

This revision supersedes DC2 for the proposed physical measurements. CPU0 timestamps pair the clock with the nested-interrupt accumulator using a retry if TIMER5 occurs between the reads. Begin/end tests inject that interruption and verify the resulting elapsed time. Frozen DC1/DC2 assets remain available for interrupted transfers; their files and pins are not overwritten.
