# SMKSYNTH 1.0.3-elite2 — SMK-37 Elite test

**Experimental. Installation and recovery have not been validated on physical Elite hardware.**

Based on public SMKSYNTH 1.0.3: SUPER on both oscillators, DRUM-X, JUMP/CATCH controls and UA1 USB audio. Does not include ENDLESS or SUPER SP1. The older `experimental/elite1` was a 1.0.2 preparation, not this page’s package.

## Install and return

1. Confirm the actual keyboard is an **SMK-37 Elite**. Pro, FM-1 and MKE-P37 are rejected, including their loaders.
2. Back up using the current firmware’s tools. The [port kit](https://github.com/genkigenki/fm1-to-smk37/blob/main/docs/02-update-and-recovery.md) documents a full flash backup including individual calibration. `.smksynth` is only a musical bank.
3. Keep **official Elite V016** from [M-VAVE downloads](https://www.m-vave.com/appdownload). Accepted source: `SMK-37 Elite_016`. Do not use Pro or FM-1 V016. An Elite identified as Pro by an older port must first return to Elite stock using that port’s installer.
4. Use desktop Chrome/Edge, close other MIDI applications and identify the device. First installation replaces/initializes the bank with five patterns and eight favourites, without extra user presets.
5. Confirm and type `ELITE`. Wait for `SMK-37 Elite_19629`. To reinstall the test while preserving saved data, choose Update from Elite `_19618` or `_19629`.
6. To return, select the official Elite V016 file downloaded from M-VAVE. Identity, size, SHA-256 and CRCs are checked before writing is enabled. This repository does not redistribute the stock file. Return may replace SMKSYNTH bank data: export it first.
7. Interrupted transfer: use the same page, browser, operation and package. Resume is bound to the hash and original source. For stock return, select the same file again. Do not switch installers midway.
8. If the test fails to start, power on holding OCT− for USB recovery. This page accepts stock return from `SMK-37 Elite_000`. Physical Elite recovery still needs confirmation. If USB does not respond, stop and consult the port kit; never use another unit’s dump.

## Volunteer checklist

- Record OS, browser and source identity; keep the full log.
- Confirm boot, display, headphones/output, USB MIDI and USB audio. iPhone video recording noise remains a known issue of the public base.
- Test all 37 keys, velocity, 16 pads, eight relative encoders, four faders in JUMP/CATCH, wheels and pedal.
- Test the five patterns, favourites, Drum Pads, Split/Layer, FX and part/bank changes.
- Save a preset, favourite and pattern; power-cycle and confirm recall.
- Export `.smksynth`, restore through the backup tool and verify sounds/sequences. The file records the Elite model and contains neither calibration nor firmware.
- Confirm return to Elite stock V016 and working controls. Only then consider the path physically validated.

## References and verified limits

- [Hardware map](https://github.com/genkigenki/fm1-to-smk37/blob/main/docs/01-hardware.md): Pro/Elite/MKE share hardware; the reference’s physical tests were performed on Pro. They do not validate our Elite build.
- [Port installer](https://genkigenki.github.io/smk37/): model names must be retained in the package, loader and return record so the corresponding stock firmware remains accepted.
- Inspected Elite V016: 705236 bytes, 20 markers, identity `SMK-37 Elite_016`, SHA-256 `b2ee38abfef3ae922e4eecdb8a551e14345300ae6f270265feacd33b6ef99cb6`. Pro V016 is a different package; it was not renamed.
- Elite2 app: 164100 bytes; static RAM 86624 bytes; pool 325984 bytes. First-install and update packages share the same app and use different loaders.
- First installation writes the application/bank area below `0x9D000`. Our loader does not write flash head `0x0000–0x3FFF`, Bluetooth/calibration `0xC0000–0xC1FFF`, USR resources or key_mac. Update preserves bank `0x57000–0x9CFFF` and refuses pending restore journals.
- Tests cover model/CRCs, 154 erase-interruption and 22 program-interruption points, byte-identical bank after update, official-return validation and `ota-SMK-37 Elite_016` record, and foreign-head refusal. These are simulated-flash tests, not physical USB/Elite validation. M-VAVE’s closed loader was not executed on hardware.

## Reproduce

Use the toolchain and SDK pinned by `tools/build.py` (JieLi AC79NN SDK V1.2.1). With `JIELI_TOOLCHAIN` configured:

```sh
python3 tools/build_elite_test.py --sdk /path/to/AC79NN_SDK
```

Both packages must reproduce the hashes in `manifest.json`. Sources, licences and credits accompany the main distribution; firmware is GPL-3.0-only with component notices preserved.
