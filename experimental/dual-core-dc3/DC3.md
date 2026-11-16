# SMKSYNTH DC3 — melodic voices on both cores

Experimental 1.2.2-dc3 /19659 for SMK-37 Pro and Elite. Official firmware remains 1.2.2 /19655. This installer only updates existing compatible SMKSYNTH installations and offers the identical preserving return packages from DC2a. It does not seed or publish a user bank.

## Why this experiment

The supplied 30-second DC2a captures showed 1C/2C full IRQ peaks of 6805/6455 us against a 5802 us half-buffer deadline, with 155/131 overruns. Drum work on CPU0 averaged 256/7 us; copying plus waiting in 2C averaged 135 us. Moving only drums left most synth and effects work on CPU0. Glitches are also reported with normal synth plus drums and no Supersaw.

DC3 replaces the drum worker with a prepared melodic voice worker. CPU0 runs all events, allocation, envelopes, controls, DRUM-X, mix and effects. It prepares all active voices before dispatch. CPU1 renders private copies of selected voices into private stereo output. Normal voices have scheduling weight 2; Supersaw voices weight 3. Selection aims at half the melodic sample cost and keeps a single voice on CPU0. This is a heuristic for measurement, not an adaptive governor or a promised speedup.

The sample-render function now takes explicit voice/part/Supersaw state pointers; its equations are unchanged. During dispatch-to-join CPU0 does not change the part globals or patch state. The suspended main loop cannot edit them, and nested Timer5 queues input/services USB without running engine control. CPU1 does not write UI, original voices, part state or drum state. On accepted completion CPU0 commits each private state and output. A rejected or failed job leaves original prepared state intact for CPU0 rendering. Normal voices copy no unused Supersaw side bank; the transition to idle still commits the complete oscillator state when the part uses Supersaw.

The transport, bounded failure wait, fault containment, flash park and private worker model remain from DC2a. Shared work is 3620 bytes (DC2a: 3528). CPU1 stack remains 7 KiB. DSP quality, sample rate, buffers, oscillator settings, effects, voice ceilings and clock are unchanged. There is no added audio-buffer latency. No hardware speedup is claimed until physical measurement.

## Measuring

1. Load pattern 9; stop playback/recording. Open GLOBE + SCALE and release both.
2. Select 1C with K1 counterclockwise BEFORE arming. Turn K2 to clear/arm. The title includes ARMED 1C.
3. Exit with PATCH, press PLAY and leave the same sequence running for at least 30 seconds.
4. Open GLOBE + SCALE. Photograph all three frozen pages, changing page with K3.
5. Exit, stop playback, reopen diagnostics. Select 2C with K1 clockwise; arm with K2 and repeat the same sequence/settings.
6. After the original pattern pair, repeat a separate pair with normal synth plus drums and no Supersaw. Keep settings identical within each 1C/2C pair; do not save experimental patch edits into the bank.

K1 remains locked while ARMED/RUNNING. If armed with the wrong mode, exit, PLAY then STOP, return and choose the next mode before rearming. FROZEN is the recorded mode; NEXT is the next mode. Stopping early freezes a shorter window. Results exclude navigation before playback and after freeze.

ANALOG/SUPER now measure CPU0 preparation plus CPU0 sample rendering only: in 2C they exclude sample rendering assigned to CPU1. DRUM CPU0 includes all drums in both modes. COPY/WAIT are CPU0 cost; CPU1 PEAK is the separate worker wall-time peak per CTL=16 frames. Average/peak stage values are per 256-frame audio half. CPU1 time overlaps CPU0 and must not be added to CPU0 times; independent stage peaks must not be summed. Instrumentation adds load and does not detect every audible glitch. CLIP SAMPLES refers only to final output clipping.

## Validation

- Deterministic dense chords with drums, with/without Supersaw: serial and pthread worker audio plus final voice, oscillator and drum state compare byte for byte. Rejected dispatch, corrupted private result and toggled 1C/2C paths match. Original DC2a serial implementation also matches.
- ThreadSanitizer and AddressSanitizer pass for both mixes, including simulated failed jobs.
- Externally supplied pattern 9: 174 seconds rendered against DC2a, DC3 1C and DC3 worker, both original Supersaw and a separate normal-SAW variant. Outputs match exactly; no private sequence, bank or audio is distributed.
- Aligned/unaligned copies and guards, profiler clock/IRQ windows, physical diagnostics controls, synth/MIDI controls and voice limits are tested. See validation/dc3.
- Both hardware packages pass app/RAM/pool/core1 stack bounds, RAM-call and HAL-access audits. Four package products, SHA/CRC, model substitution guards and interrupted install/return scenarios pass.
- Backup allows exactly audited identity 19659 for Pro/Elite with unchanged SMK16P1/B2R1. Native protocol export/restore/retry/resume is tested for both identities.

Build: Jieli toolchain 20260730.1, SDK AC79NN_SDK_V1.2.1_2023-12-13 (three packaged SDK components hash verified), --voice-hardware, OBV_DEFS=-DOBV_DUAL_CORE=1, --release 1.2.2-dc3 --experimental-id 19659. No --first-install or seed bank. Source: experimental/dual-core-dc3 branch.
