# SMKSYNTH 1.0.2-ss3 — High-note processing revision

Hardware report from Marcus: one SUPER now plays without stalls and O remains unchanged. SUPER + SAW + FX showed some late high notes, without noise. Two SUPERs still stalled; the photo shows P=6727 us and O=569. The nominal render deadline is 256/44100 s = 5805 us, so that configuration measurably misses deadlines. The high-note input delay is consistent with reduced main-loop time, but its cause is not separately proven.

## SS3 changes

1. Replace each saw-wrap division with an exact cached reciprocal plus remainder correction. This is integer-exact, not an approximate waveform. High notes wrap more often, so removing wrap divisions targets their pitch-dependent cost. Reciprocal values are recalculated only when their denominator changes, not on every tiny drift change.
2. Build only `obv_supersaw.c` with -O2 in a separate object. Main synthesis/control/USB keep their tested -Os options. `obv_osc_type.h` shares the oscillator ABI; host/browser builds still include the same implementation directly. Native tests also exercise an external -O2 kernel linked to an -Os controller fixture.
3. When a voice contains SUPER plus ordinary SAW, render the companion SAW with the same optimized natural-wrap kernel. Voices with no SUPER retain the original path. SYNC/XMOD remain bypassed in SUPER voices as before.
4. Compact the SUPER timing label to `P6727 O569` so it fits alongside the page title. P is peak audio-render microseconds; O is the accumulated deadline-overrun count. Restart clears the counters.

There are still seven saws per SUPER, all eight shared voice cards, the same detune/gain laws and same saved patches. No sample-rate reduction, voice cap or note delay is deliberately introduced.

## Validation

- Existing SS1 reference comparison still passes: 32768 paired blocks, every MIDI note, parameter/pitch changes, silent advancement, oscillator states and samples exact.
- 1572840 reciprocal quotient cases match integer division; 32768 companion-SAW blocks match the original oscillator, including high notes and silent advancement.
- Whole-engine comparison against SS2: 48 repeated chord attacks over four scenarios (4/8 notes, high notes, release overlap, modulation/drift, SUPER+SAW, dual SUPER, silent OSC2 and effects). The PCM streams are byte-identical: SHA-256 `7c7688bb6186498d7f9abe83db713e8ef7f4c98b69bfe094007965aa50471fba`. An external -O2 kernel with an -Os host main also produces that hash.
- ASan/UBSan and existing physical-control/sequencer/layer/scene tests pass. The target build checks RAM/flash bounds and no outbound calls from RAM. Target disassembly has no divide instruction inside any rendering loop of either optimized kernel.
- Installer and preserving-loader tests cover SS2->SS3, interruption/resume and stable return. Original SS1 and SS2 resume pages/keys/packages are preserved.

See `validation-ss3/` for logs. Host timing ratios do not predict keyboard CPU; reduction of P/O and elimination of late high notes/stalls still require the physical test.

## Install and compare

Update directly from SS1/SS2 using the current installer. SS3 identity: `SMK-37 Pro_19621`. SUPER presets/favorites/scenes/backups are unchanged and carry over. No restore is required when upgrading. Keep a backup. Stable return remains available; stable 1.0.2 does not understand SUPER patches, so use a pre-SUPER backup when returning to it.

Restart between measurement cases. First repeat one SUPER + SAW + the same effects and high-note chords. Then use two SUPERs with the same settings that gave P6727/O569. Note new P, whether O grows, whether the stall remains and whether high notes arrive late. P excludes some USB/output work: O=0 alone does not guarantee enough control-input margin.

Build uses the usual SDK/toolchain with `--release 1.0.2-ss3 --experimental-id 19621`; the build script compiles the extra DSP object automatically. USB implementation and its compiler profile remain unchanged. iPhone USB-video clicks remain the separate known issue.

---

## Previous revisions and architecture

# SMKSYNTH 1.0.2-ss2 — Supersaw performance revision

SS2 follows hardware feedback: SS1 sounded good but simultaneous attacks of four notes stalled; a second SUPER made it worse. This is consistent with missed audio deadlines, but the physical cause is not proven without timing measurements. No voice cap, dropped saws, lower sample rate or altered detune/gain law is used here.

## What changed

- Render one saw across the block with phase/previous sample in registers, then move to the next; specialize the natural-wrap correction algebra exactly.
- Cache frequency steps and gain normalization when their inputs have not changed. Pitch changes/drift invalidate frequency cache; smoothing and detune law remain identical.
- Replace divisions by 16 and 6 with exact truncating arithmetic. The target compiler otherwise emits divides even for constants under -Os.
- Execute the hot routine from `.ram_text.super`, using the existing platform's far-call pattern. Its offset table is in RAM. The compiler/link checks confirm no calls out of the RAM kernel.
- If MIX is exactly zero, advance six inaudible sides directly; if oscillator LEVEL is zero, advance all seven directly. Final oscillator states match a full render exactly, so raising MIX/LEVEL resumes at the same phases.
- Show P (peak render microseconds) and O (number of render blocks exceeding the nominal 5805-us budget) on the SUPER page. Counters reset on boot. P measures synthesis/effects, not all USB/output overhead.

## Evidence and limits

`tests/supersaw_performance_test.c` compares against the frozen SS1 renderer in `tests/reference/supersaw/ss1.h`: 32768 paired blocks over every MIDI note, varying pitch, detune and mix, variable block sizes, silent fast-forward and wave restarts. Both output samples and oscillator phases/delayed state are bit-identical. Tests also exhaust the relevant signed ranges for the replacement divisions. ASan/UBSan passes; existing controls/sequencer/layer/scene regressions pass. The legacy host WAV still hashes to `09998294144d7327cac2e74aa30d90417e3a0905ba8c162060843f79ff5ca441`.

Target build: app 163156 bytes; data+bss 86208/98304 bytes; pool 325984/344064; RAM kernel has no outbound calls. Host microbenchmarks are included for transparency: different optimization/sanitizer settings give different ratios, and they do not predict keyboard CPU. **No hardware performance claim is made yet.**

## Install and reproduce

Use the current private test installer. SS2 identity is `SMK-37 Pro_19620`; it accepts stable 19618, SS1 19619 or SS2 19620. Stable return accepts SS1 and SS2. Transfer resume is pinned to SS2 separately; the original SS1 page/keys/packages are preserved under `legacy-ss1/` for interrupted SS1 transfers.

SS1 saved SUPER presets/favorites/scenes/backups are compatible with SS2: all saved layouts and the SS extension are unchanged. No restore is required to upgrade from SS1. Keep a backup before testing. Stable 1.0.2 still does not support saved SUPER patches, so use a pre-SUPER backup if returning to stable.

Repeat the exact four-note attack first with one SUPER (OSC2 OFF), then two. Observe P/O before and after attacks. Test 3/4/6/8 simultaneous notes and sustained releases. Then add effects, layer/sequencer/DRUM-X. Report whether the stall remains and the P/O readings for the failing combination. A new boot clears the peak/counter between cases. This comparison distinguishes deadline misses from another fault without changing the approved sound.

Build: same command below, but `--release 1.0.2-ss2 --experimental-id 19620`. Known SS1 limitations (SYNC/XMOD bypass and no PWM on detune) remain.

---

## Original SS1 architecture and test notes

# SMKSYNTH 1.0.2-ss1 — Supersaw experimental

Base: stable 1.0.2, commit `4ab63cf6c5e04873833b9524e4dac0e3e383e156`. Branch `experimental/supersaw`. SMK-37 **Pro only**. Not a first install; requires an installed SMKSYNTH (identity 19618). Experimental identity: `SMK-37 Pro_19619`. Return: original stable 1.0.2 update, identity 19618.

## Controls / Controles

Select ANALOG, then PARA → OSC1 / OSC2 → WAVE SUPER. PW becomes DETUNE (0–127). PARA cycles OSC1 → OSC2 → OSC MOD → FILTER → FILTER + → SUPER. On SUPER: K1 DET1, K2 MIX1, K3 DET2, K4 MIX2. MIX=0 plays only the central saw. Fine/semitone tuning, pitch bend, glide, vibrato, levels, filters, envelopes and sends retain their roles. The usual UNISON stacks complete voices and is separate from the seven saws per oscillator.

SUPER uses seven event-corrected saws at uneven frequency offsets derived from Clipper HYBRID / Adam Szabo's analysis. Clipper's simple detune curve is retained. MIX is independent; the sum is peak-normalized to control saturation (perceived loudness can fall as phases spread). Controls slew over approximately 6 ms. Side phases start decorrelated and remain free-running. This is not a JP-8000/8080 emulation; the Roland's dedicated pitch-tracked high-pass is not included.

SS1 bypasses both hard sync and X-MOD whenever either oscillator is SUPER. The UI indicates this. PWM does not modulate SUPER's detune yet. MIX is saved per oscillator but has no dedicated CC or sequencer automation lane in SS1. DETUNE reuses the existing PW parameter/CC/automation; a recorded PW lane therefore controls detune while SUPER is selected.

## Data and rollback / Dados e retorno

Export a backup before testing. Updates and return use the existing preserving loader. The ordinary installer does not accept SS1's identity; use this experiment's return operation. Stock V016, Elite, MKE and FM1 are rejected. An interrupted update is resumable only in the same browser, with its matching pending operation and pinned package.

Patch size stays 80 bytes, NPARAM=63, name at bytes 63–72, keyboard octave at 73. ANALOG MIX uses the six trailing bytes 74–79 as `SS`, version 1, MIX1, MIX2, XOR checksum. Missing/invalid SS extension reads MIX=64 without writing. DRUM-X's original OVE tag remains unchanged. Names, CC assignments and record sizes retain their positions. Presets, favorites, scenes and backups carry the extension as part of the patch. MIX writes are protected against audio-ISR interruption.

Stable firmware does **not** support WAVE=4. Its favorite validator may reject a saved SUPER favorite bank; it may interpret a SUPER oscillator as silent. Returning the executable preserves the bank; restore the backup made before SS1 to undo saved SUPER data. A backup made during SS1 is for restoring in SS1. Unsaved experiments can simply be discarded by recalling an original preset.

## Build

Use the original toolchain and SDK requirements in `docs/BUILD.md`:

```sh
JIELI_TOOLCHAIN=/path/to/jieli-toolchains python3 tools/build.py \
  --voice-hardware --board smk37 --model pro --release 1.0.2-ss1 \
  --experimental-id 19619 --sdk /path/to/AC79-SDK
```

The SDK components used here were recovered from the project's released 1.0.2 package and matched all three SHA-256 values pinned by `tools/build.py`; no SDK content is added to this branch.

## Validation / Validação

Automated: all 128 MIDI pitches, detune endpoints, exact central-saw output at MIX=0, bounded oscillator output; independent detune/mix encoder edits; page cycle and DRUM-X context; unchanged patch/name/octave layouts; program/favorite/flash scene recall; eight shared voices/four parts with two SUPER each; explicit SYNC/XMOD bypass. SS1 test passes ASan/UBSan. Existing controls, sequencer, patterns, split/layer, scene-recall and boot-header regression suite passes. Loader tests cover preservation, interrupted transfers and stable return. Installer verifies SHA-256/CRC/model before any write and tests interrupted resumes with a simulated MIDI updater.

The existing host regression WAV is byte-identical to stable 1.0.2: SHA-256 `09998294144d7327cac2e74aa30d90417e3a0905ba8c162060843f79ff5ca441`.

Stable app: 160424 bytes; SS1 app: see `validation/firmware-build.log`. RAM data+bss rises from 84576 to 85536 bytes (+960); limit 98304. Pool remains 325984/344064 bytes. Compile/layout checks are **not** a measurement of CPU use on the keyboard. CPU and USB recording behavior with SUPER remain physical tests.

## Hardware test order / Roteiro no teclado

1. Backup → install SS1 → confirm boot version 1.0.2-ss1.
2. ANALOG POLY, OSC2 OFF, effects off: play low/high notes; sweep DET1 and MIX1. MIX=0 should be a single saw.
3. Add OSC2 SAW, then SUPER; adjust oscillator FINE/SEMI and levels.
4. Play chords and sustain with long releases. Repeat with layer, sequencer, DRUM-X and effects. Inspect GLOBAL render peak and overrun indicator. Report exact combination, peak and any artifacts; software tests do not guarantee eight notes on hardware at every setting.
5. Save/recall a disposable preset/favorite/pattern, reboot and verify.
6. Check the already-approved Mac/AudioShare USB paths. Existing iPhone video clicks are still a known issue, not fixed here.
7. Return via this installer's stable button; restore pre-test backup if SUPER data was saved.

Credits: SMKSYNTH / Marcus Padrini and ChatGPT; OberVave / zvenson and contributors; Clipper / 3dSam, after Leo Kuroshita / Hügelton Instruments; Adam Szabo's analysis. GPL-3.0-only; see root LICENSE and LICENSING.md. Source: https://github.com/4t59m4f784-creator/SMKSYNTH/tree/experimental/supersaw
