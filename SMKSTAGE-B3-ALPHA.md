# SMKSTAGE B3 — Hardware Alpha 0.1.2-b3

**EXPERIMENTAL / NÃO TESTADO EM HARDWARE PARA 0.1.2**. The previous 0.1.1 was reported playable on a physical SMK-37 Pro; 0.1.2 includes a new full-rate plate DSP, a new flash journal, updated Leslie/drawbar controls and factory presets, which are not yet verified on real hardware. This is **not a public production firmware**.

## Source attribution and build

- Hardware platform / MIDI / CPU1 processing: SMKSYNTH sources on this isolated development branch, GPL-3.0-only.
- Organ engine: R-Jeb's setBfree-fm1 revision `4dd709b18b24cb5934dd57796ac65940722d9383`, based on setBfree, GPL-2.0-or-later source modules where indicated.
- Original B3 Leslie/tonewheels/drawbar mapping retained. The legacy setBfree-FM1 comb reverb is excluded under `STAGE_PLATE=1`.
- New stereo plate: `firmware/src/smkstage_plate.h`, based on SMKTRON's port of OberVave's Dattorro reverb (zvenson; credits embedded).
- Hardware build: `python3 tools/build_stage.py --board smk37 --model pro --experimental-id 19703 --release 0.1.2-b3`.
- Build pipeline: `.github/workflows/smkstage-alpha.yml`; DSP host tests: `smkstage-host.yml`.

## Physical workflow

- **Performance page:** K1–K8 control drawbars 1–8; F1 controls drawbar 9 directly, bottom=0/top=8 (no pickup), F2 swell, F3 drive, F4 reverb.
- **PARA:** cycles Performance → Leslie Speed → Leslie Ramp → Tonewheels → Drive → Plate → EQ → Vibrato/Percussion. **PATCH:** back to Performance.
- Leslie Speed K1–K4: Horn Slow/Fast, Drum Slow/Fast in 0.1 RPM.
- Leslie Ramp K1–K4: Horn Acc/Dec, Drum Acc/Dec in milliseconds.
- Tonewheels K1: 430.0–450.0 Hz tuning (0.1 Hz encoded; increments of 1 Hz); K2: key-click level 0–1000. Leakage comes from fixed upstream setBfree tables, with no exposed runtime control.
- Drive K1 level, K2 character. Plate K1 size, K2 tone, K3 pre-delay, K4 mix. EQ K1..K6 low/mid/high gain and frequency. Vibrato K1 mode, K2 Upper, K3 Lower, K4 Perc.
- Pad 9 Leslie bypass (separate from STOP); pad 10 tap Slow/Fast, hold Stop. Pad 11 percussion, Pad 12 soft, Pad 13 fast, Pads 14–15 vib/chorus, Pad 16 overdrive.
- Pads 9/10 at maximum brightness; pad 10 uses the actual horn phase, not the former slow synthetic clock.

## Presets

The first boot seeds 8 musical registrations from setBfree in **both** pad banks: 5 Standard B, 104 Jimmy Smith, 7 Gospel, 15 Booker T Jones, 18 AWSOP, 107 ELP Keith Emerson, 106 John Lord (as named upstream), 108 Paul Shaffer.

- Bank A: drawbar registrations, restores the three manuals' drawbar positions without replacing the other effects.
- Bank B: complete favorites; captures all three manuals, percussion, Leslie speed/ramp, stage-specific bypass, tonewheel tuning/key click, drive and character, stereo plate settings, EQ, vibrato and swell.
- **KNOB BANK** switches banks. Pads 1–8 short press load; hold >=600 ms saves.
- Program library browser, custom names and user slots 112–127 are future work, not implied by the eight seeded favorites.

## Persistence and flash safety

**Stage-only journal sectors: `0xE8000` and `0xE9000`.** These are after SMKSYNTH's four-track slots ending `0xE1FFF`, in the documented 0xC2000–0xE9FFF user-sample window. The new flash write allowlist enables only these two additional 4 KiB sectors, not the stock protected BT calibration and user resources. The automatic SMKSYNTH owner erasure remains disabled.

- On boot, both sectors are read and verified (DNGY sector CRC and versioned `STAGE_TAG`). Latest valid generation wins.
- A sector is writable only if fully erased or holding a valid Stage record. Unknown or incomplete writes are **never overwritten**.
- Writes alternate between sectors. Read-back + CRC and full byte comparison required before reporting **SAVED**. If the new sector is interrupted and the previous valid copy survives, it may be restored read-only, displaying **FLASH LOCKED**.
- Press **BT** (SAVE) while all notes are released to request a write. Dirty state also auto-saves after 10 seconds without edits while `quiet()`, with at least 30 seconds between automatic writes. **Do not power off until SAVED is shown**. Save can cause a brief audio interruption; it only occurs when the organ is silent.
- The sector geometry is based on the currently known SMKSYNTH/SMKTRON maps; **safety against unknown third-party firmwares is not guaranteed**. Unknown data is detected and blocked, not erased.
- If the device reports FLASH LOCKED, do not try to erase arbitrary flash; preserve a log.

## Real hardware test gates

1. Confirm 0.1.2 timing and polyphonic performance versus proven-playable 0.1.1, especially Dattorro plate at long tail.
2. Confirm drawbar F1 direction, full range and fade-free K8; physical K1–K8 and PARA pages.
3. Confirm Leslie controls, actual pad-10 pulse, percussion on Upper manual.
4. Confirm SAVE success, power-cycle restoration of drive/Leslie/plate/EQ/favorites and behavior under interrupted save. Never intentionally interrupt erase to test without a recovery plan.
5. Verify flash ownership and compatibility with other installed firmware on this specific keyboard.
6. Confirm bootloader rescue and rollback. Use isolated old installer at `https://4t59m4f784-creator.github.io/SMKTRON/smkstage/install-011/` to install the last playable 0.1.1 with its pinned .fwsc and original V016.

The separate installer in `SMKTRON/web/smkstage/install` is currently pinned to this exact alpha and never silently fetches or selects the .fwsc for users. All code and licenses must remain public with required original credits.
