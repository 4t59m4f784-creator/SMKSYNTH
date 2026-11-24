# SMKSTAGE B3 — Full Feature Alpha 0.1.3-D2

EXPERIMENTAL AND NOT HARDWARE VALIDATED. Previous 0.1.2 booted to USB Rescue. New D2 includes startup audio gate but its effectiveness is unproven. Official SMKSYNTH and SMKTRON are unchanged.

## Full feature list
- All nine drawbars: K1–K8 and F1. F1 position 127 at top = drawbar 0, position 0 at bottom = drawbar 8, with direct response (no pickup).
- F2 Swell, F3 Drive, F4 Reverb. MOD wheel Fast at the top / Slow at the bottom, with 520/580 ADC hysteresis; stationary wheel does not override pad-selected STOP.
- PARA cycles nine pages: Performance, Leslie Speed, Leslie Ramp, Tonewheels & Key Click, Drive, Stereo Dattorro Plate, Three-band EQ, Vibrato/Percussion, SetBfree Preset Library. PATCH returns to Performance.
- Leslie Speed K1–K4 sets Horn Slow/Fast and Drum Slow/Fast. Leslie Ramp K1–K4 sets four independent acceleration/deceleration constants.
- Key Click and tuning exposed. Leakage retained from setBfree tables; no independent control in source engine.
- Stereo Dattorro port from OberVave via SMKTRON, mixed after Leslie; adjustable size, tone, predelay and wet amount.
- Bright LED pads: pad 9 bypass full intensity, pad 10 real horn rotor-phase pulse up to RGB 255.
- Eight seeded musical registrations in Bank A and eight full sound favorites in Bank B; KNOB BANK chooses A/B; short pad tap loads and hold >=600ms stores.
- Factory setBfree musical library: PRESET LIBRARY page, K1 browses and immediately loads original musical registration with the correct ID and name. Hold a Bank B pad to capture a snapshot.
- Two-sector 0xE8000/0xE9000 CRC journal, verifies owner before erasing or writing, explicit BT Save or quiet auto-save. SAVED/UNSAVED/FLASH LOCKED status on-screen. Real-device persistence remains unverified.
- D1 incremental canvas updates for changed drawbar lanes, seven-field crash report (VEC/PC/RETS/DBG/EMU/T63/SP), and stage_audio_ready gate before audio ISR calls new plate effects.

## Forensic context
Previously observed hardware crash: VEC 1, invalid PC 0x030004A8 (one-bit difference from valid timer ISR instruction 0x020004A8), RETS in audio ISR at multicore dispatch. Exact 0.1.1 rebuilt and symbolicated. Root cause not proven. Avoid changing IRQ priorities or keybed scan behavior until verified.

## Build
GitHub Actions D2 full-feature compile was successful, release name 0.1.3-d2, 651339-byte SMK-37 Pro_19703 package.
SHA-256: 4eb116f58c014ed0906ac7372b1f537bae1801cd7e4887b4925f0c51d8a3ffd2
Branch: https://github.com/4t59m4f784-creator/SMKSYNTH/tree/smkstage-b3-d2-complete-alpha
Build run: https://github.com/4t59m4f784-creator/SMKSYNTH/actions/runs/45330992816
Dedicated pinned experimental installer: https://4t59m4f784-creator.github.io/SMKTRON/smkstage/install-d2/
Recovery to physically tested 0.1.1: https://4t59m4f784-creator.github.io/SMKTRON/smkstage/install-011/

## Real hardware test steps
1. First boot: check normal display, low-latency notes and no Rescue. Do not start a save until boot and audio are stable.
2. Test D1-style drawbar update and quick encoder movement with chords, watching for crashes and audio glitches.
3. Test F1 direction, MOD wheel, real Leslie pads, custom speeds and ramps, key-click, Dattorro and EQ.
4. Enter PRESET LIBRARY via PARA, browse with K1 and hold a Bank B pad to store a snapshot.
5. Only after stability checks, save in silence using BT, wait for SAVED, power-cycle and check all settings; FLASH LOCKED means no save and should be reported.
6. If USB Rescue, use the 0.1.1 fallback installer from the same browser; do not reflash D2 and preserve logs.

## Source credits
setBfree-fm1 by R-Jeb and upstream setBfree contributors; OberVave / zvenson Dattorro plate via SMKTRON, and SMKSYNTH/JieLi port contributors. Original GPL notices retained in code.