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
