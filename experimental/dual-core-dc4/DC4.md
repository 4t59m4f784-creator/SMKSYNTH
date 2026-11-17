# SMKSYNTH DC4 — exact DSP cost reduction

Experimental 1.2.2-dc4 /19660 for SMK-37 Pro and Elite, based on DC3 source db9494718138a99f242a10b59c60c8cd22fdf9b0. Official remains 1.2.2 /19655. Installer provides preserving update, return to the exact frozen DC3 /19659 package, and return to the exact official /19655 package for each model. Nothing overwrites DC3's installer/assets or a user bank.

## Changes

- CPU0-only voice filter coefficient memoization: retain c1/c2 only when the exact clamped cutoff and damping match the inputs to the original svf_coef. Any changed key is recalculated at the same control boundary. c2 is used only where the original F_LP24 path uses it. Cache validity clears at voice start, hard panic and engine init. Envelopes and per-sample filter states continue advancing. This cache never travels in CPU1 snapshots and does not change voice_t, saved layouts or worker memory.
- Fixed-power-of-two circular addressing for the existing delay and chorus interpolation points. Masking substitutes for the old valid single-wrap arithmetic; linear/Hermite equations and sample order stay identical. Static assertions constrain the relevant lengths. Reverb line lengths and equations are unchanged.
- Pure fx_process/drive_process/master_process in a separate -O2 -fwrapv object. Control/init and all states remain in main; these routines run on CPU0. UI, USB, storage and main retain -Os. Static DSP helpers and existing fixed-point constants are shared. -fwrapv retains two's-complement wrap arithmetic in the optimized object; it does not approximate the equations.

DC3's prepared normal/Supersaw worker, snapshot recovery and transport are unchanged. No extra block latency, half-rate reverb, changed oscillator counts, shortened releases, reduced voice ceilings, changed clock/buffers or removed USB audio. The physical residual pops reported in DC3 are not yet explained or claimed fixed.

## Evidence and limits

The externally supplied pattern 9 renders 174 seconds with original Supersaw and a separate ordinary-SAW version. DC4 1C and 2C match original DC3 sample for sample in both mixes, including final drum state. The user bank, records and audio are not distributed.

Instrumented host calls in either mix: 1,613,481 coefficient evaluations and 3,786,567 exact reuses, avoiding 70.12% of these evaluations. **This is not a 70.12% total CPU reduction.** Savings depend on patch modulation and playing; constantly changing cutoff/resonance yields fewer hits. Filter sample processing is unchanged.

Isolated FX stress fixture: original DC3 -Os, DC4 inline reference and separate optimized object emit identical stage samples, delay memories, write indices and all semantic FX/master states across control sweeps, all drive types, compressor/filter/limiter, tails and sizes 0..256. SHA-256 736df305f2ef0b0568e7e1f9f1c0bb59c25d42a280b83cf29ce0c3ef47ed4825. ASan passes. Exhaustive chorus index/offset and fractional-boundary checks plus delay wrap/offset sweeps and 1M random fractional taps match old interpolation.

100k cache key/voice-clear transitions match original svf_coef results exactly. Dense chords with drums in serial/pthread paths match PCM and complete voice/oscillator/drum states, including rejected dispatch, corrupted private results, toggling 1C/2C and with/without Supersaw. TSan/ASan pass with the separate object linked. MIDI/tuning/panic/controller transitions and scene/release tests also link the external kernel.

FX-only host benchmark did **not** establish a speed improvement (DC3 approximately 1.13 s vs DC4 1.16 s in one CPU-time run). Host timings are not target timings. The cache is the demonstrated reduction in work; the effects changes still need measurement on the chip. Do not infer a real-time improvement from static code size or the host call count alone. Firmware/RAM/pool/core1-stack audits pass for both models.

## Hardware comparison

Keep the same pattern, effects and **2C** in both DC3 and DC4. Use the existing 30-second capture: stop transport, GLOBE + SCALE, choose 2C with K1 before arming K2, PATCH then PLAY, and photograph all three frozen pages. Prefer the recorded sequence for repeatability, then test live chords with the sequenced drums. Compare ANALOG/SUPER, FX+MASTER, IRQ PEAK/OVER and audible pops. Do not sum unrelated stage peaks or add CPU1 time to overlapping CPU0 work.

A preserving DC3 return is available in this page for A/B comparison. Official return is a separate button. K1 remains locked while ARMED/RUNNING. The title now identifies DC4; controls, capture duration and stage definitions are unchanged. Backup recognizes Pro/Elite identity 19660 with unchanged SMK16P1/B2R1 and native export/restore/retry/resume tests.

## USB audio

USB audio remains enabled. Its helpers return immediately when no stream is active. Active capture adds conversion/ring/packet work; at 48 kHz it also invokes the existing resampling FIR. SCAN+USB IRQ combines key scanning and nested USB service and is not an isolated measurement of USB audio cost; uac_render_start/uac_tap also contribute outside it. A MIDI-only build would be a separate controlled experiment, not part of this change.

Build: Jieli 20260730.1, SDK AC79NN_SDK_V1.2.1_2023-12-13 with packaged SDK hashes verified, OBV_DEFS=-DOBV_DUAL_CORE=1, --voice-hardware --release 1.2.2-dc4 --experimental-id 19660, Pro/Elite. No first-install seed. Validation: validation/dc4; sources: experimental/dual-core-dc4 branch.
