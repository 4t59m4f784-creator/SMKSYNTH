# SMKSYNTH 1.2.2-dc1 / 19656

Experimental, based on official Endless MIDI 1.2.2, commit `9433886228f2c24bd3cac1621ae8de274e3c6fdd`. This is not a public-release promotion. Existing SMK16P1 bank, scene, pattern, preset, MIDI and theme layouts are unchanged. Both packages are preserving updates for existing SMKSYNTH users, not first installs from stock firmware.

## Findings and choice

OberVave main at `a1f660c4439583d33f72941bd184a7e187d11835` (3.5.2) still renders audio on one core. Its active firmware/build sources have no CPU1 startup or audio dispatch. JIANT main at `da8baa1eed7c4e75f1ed9bcf433797b3aeace107` supplies the CPU1 transport used here. It matches the transport in the previously inspected Jazzy source archive. JIANT credits Melodee and fm1-x0x for the AC79 startup and fault-containment work.

A broad transplant of Jazzy's mixer/governor would change allocation, effects and sound behaviour. DC1 instead offloads the busiest active DRUM-X track (count of active/triggered instruments) per 16-frame control block. CPU0 processes events, MIDI, sequencers, patch changes and controls first, snapshots that kit, submits it to CPU1, and concurrently renders Obervave/Supersaw and all remaining drum tracks. It waits, commits and mixes in the original frame interval. No extra block is inserted. One drum-only track has little parallel work to overlap; its gain may be negligible or negative. Multiple drum tracks still use both cores because the non-selected kits run on CPU0 before the wait.

CPU1 receives private drum state, parameter copies, peak meters and output buffers. It does not access the live patch or voice allocation/mixer state. Only a completed job commits state/output. On a rejected job, CPU0 renders the original; on timeout/fault, the transport holds CPU1 and CPU0 renders from the untouched original snapshot. This avoids committing a half-advanced hit or double-advancing a retrigger. The transport checks function/argument/job sequence but does not prove freedom from all possible silent RAM errors.

The worker idles in RAM, with a separate 7 KiB stack allocation. Flash entry parks the worker, and reset/update paths stop CPU1. Startup faults and watchdog resets can keep CPU1 disabled; full power-off clears the transport's retained bar under the same reset-reason logic as the platform. Existing OCT- recovery remains. The bounded job timeout is 2 ms; a fault recovery itself can miss an audio deadline. Normal hardware timing has not been established.

## What is unchanged

No DSP equation, sample/control rate, oscillator, drum table, filter, envelope, gain, effect algorithm, voice limit, buffer size or CPU clock was intentionally changed. SUPER remains restricted to OSC1 and retains its existing ceiling. No automatic quality reduction/governor was imported. Polyphony expansion is deferred until physical measurements show the available headroom.

## Diagnostics and A/B test

1. Play the same problematic pattern with the same volume and effects.
2. Press GLOBE + SCALE, then release both. This opens the DC1 diagnostics overlay.
3. K1 counterclockwise selects 1C, clockwise selects 2C. This selection is session-only and applies between audio jobs. CPU1's idle loop remains running in 1C comparison mode.
4. Turn K2 to reset the counters before each equally long test (suggested: 30 seconds). Do not change patches/effects during the timing window. Photograph each result.
5. PATCH exits. Start/stop or edit normally outside the overlay. The same diagnostic display/refresh runs in each comparison mode.

IRQ PEAK measures the full ALNK0 ISR including nested scan/USB work and output conversion. IRQ OVER counts ISR executions whose elapsed time exceeds the DMA half period. It is not a direct counter of every missed DMA event or audible glitch. CLIP SAMPLES counts output samples clamped after the master gain; it does not detect clipping earlier in the DSP. CPU1 JOBS counts accepted submissions. CPU1 PEAK measures worker render time, WAIT PEAK measures post-render wait/commit entry time, FALLBACKS counts rejected/failed jobs rerendered on CPU0. The worker's normal returned processing time and wait peak use TIMER4 (24 MHz).

If CPU1 is unavailable, the original serial renderer remains active. The comparison overlay is experimental and shows English engineering labels. The web page includes Portuguese instructions and an English quick guide.

## Reproduce

Dependencies: the repository's pinned AC79 SDK, JieLi Linux toolchain, Python/Pillow, host C compiler and pthreads. The build retains `-Os` for the unity app/USB and `-O2` for the existing separate Supersaw unit.

```sh
OBV_DEFS=-DOBV_DUAL_CORE=1 JIELI_TOOLCHAIN=/path/to/toolchain \
python3 tools/build.py --voice-hardware --board smk37 --model pro \
  --release 1.2.2-dc1 --experimental-id 19656 --sdk /path/to/sdk
```

Repeat with `--model elite`. Do not add `--first-install` or a factory bank. Model identities differ and cannot be interchanged. `manifest.json` pins each size, identity and SHA-256, including the unchanged official 1.2.2 preserving return packages.

## Validation completed

- Native serial vs actual pthread worker, rejected submissions, corrupted/failed private jobs and live 1C/2C toggles: identical PCM plus final drum state in all tested sessions. 27,696 accepted jobs in the all-worker session; five kits, dense retriggers/chokes, two/four drum tracks, analog, SUPER, parameter changes, engine changes and panic included.
- Same session against a clean official 1.2.2 checkout: identical PCM/state. SHA-256 `30a0109ffec4af806d0b6cab29d279864e538acdafe8011dcc531e9482a9659e` includes test PCM and final drum state, not a firmware hash.
- AddressSanitizer and ThreadSanitizer passed the threaded host session (including injected private-result failure for ASan). This does not test the chip's cache/bus/core startup behaviour.
- Existing DRUM-X native test passed. Physical-control/MIDI regression suite and Vintage/Detune MIDI-return/PANIC regression passed on the host.
- Pro/Elite target compilation and static image/RAM/pool/stack/MMIO checks passed. Worker RAM-loop section is copied independently from the flash driver's call-free RAM section.
- Four installer packages pass model/identity/SHA/CRC checks; journal/private-bank areas are unseeded; mocked interrupted update and return workflows passed, including wrong-model and stock-firmware rejection.

No installation, listening, load benchmark, USB audio or controls test on a physical SMK-37 was performed by the agent. No numeric CPU gain, glitch elimination or higher voice count is claimed. Physical 1C/2C results on Marcus's problematic pattern decide the next experiment.
