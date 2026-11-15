# Building SMKSYNTH 1.2.1

[Português](BUILD.md) · [English](BUILD.en.md)

Identity: `SMK-37 Pro_19653`. Runtime base: approved Endless MIDI A commit `11d0aa566c079342d7a33fc3e7b382e64f92104e`. Synthesis, sequencing, interface and data formats are preserved from that base.

## Requirements

Linux x86-64 (or the original scripts’ Docker flow), JieLi pi32v2/common toolchain, Python 3 and AC79NN SDK. The simulator also needs Zig and Binaryen wasm-opt. Component references and licences are in [LICENSING](../LICENSING.md). The build verifies hashes for the SDK components used.

## Factory content

`factory/` contains five selected patterns and scenes, eight favourites and clean initial settings. It excludes the original full private backup and its three user presets. Two patterns retain a legacy SEQ4 prefix supported by the firmware’s normal compatibility path.

```sh
python3 tools/build_factory_bank.py factory factory
python3 tests/release_factory_test.py
```

The author’s backup is not needed to rebuild. `tools/prepare_factory_seed.c` documents edited badges relative to base presets, initial favourite generation and clean settings at 90 BPM. Notes, patches and mixer values are preserved.

## Firmware

```sh
export JIELI_TOOLCHAIN=/path/to/jieli-linux-toolchains
python3 tools/build.py --voice-hardware --board smk37 --model pro \
  --release 1.2.1 --identity 19653 --first-install \
  --factory-bank factory/bank-SMK16P1.bin --sdk /path/to/SDK
```

Output: `build/obervave-1.2.1-smk37.fwsc`; distribution name: `SMKSYNTH-1.2.1-first-install-SMK37-Pro.fwsc`.

The first-install profile uses the platform’s standard full-area loader to initialize `0x4000..0x9CFFF`, including the SMK16P1 seed. The application retains SMK16P1 storage. The browser requires official V016; this is not the SUPER SS3 bank-preserving updater.

The restore journal starts erased at `0x55000..0x56FFF`; the seed occupies `0x57000..0x9CFFF`. No boot-file CRC includes this data area. Boot header, calibration and protected manufacturer regions are not written. The loader’s original sector comparison/verification and resume flow remain in use.

## Simulator and web distribution

```sh
SIM_VERSION=1.2.1 sh tools/build_controls_sim.sh
cp build/controls/SMKSYNTH-1.2.1.wasm web/simulator/obervave-voice.wasm
node tests/release_demo_wasm_test.mjs
node tests/release_110_installer_test.mjs
node tests/release_i18n_test.mjs
python3 tools/package_release.py build/public-site
```

The simulator uses `factory/simulator-session.bin`, representing the same musical records in host memory format, separately from physical flash. `web/site/i18n.js` translates browser UI only; saved music, protocol fields and firmware controls are unchanged. Do not run the legacy web preparation script over edited final pages without reconciling its output.

To assemble the site using already frozen release downloads without repacking them:

```sh
python3 tools/package_release.py build/public-site --reuse-release
```

The GitHub Pages workflow uses that mode. The release workflow uploads the committed firmware, corresponding-source archive, metadata and SHA256SUMS. [Validation](VALIDATION.en.md) records tests and limits.

## Preserving update / Atualização com preservação

Use the same build command without `--first-install --factory-bank factory/bank-SMK16P1.bin` to build the bank-preserving update. The first-install and update packages are separate; do not interchange them. `obv_supersaw.c` is compiled at `-O2`; the main application and USB paths retain `-Os`. SUPER attribution: [Clipper](https://github.com/isod89/clipper-fm1); see LICENSING.md.

## 1.2.1

`python3 tools/build.py --board smk37 --model pro --release 1.2.1 --identity 19653 --voice-hardware --sdk /path/to/sdk`

Elite: `--model elite`. First install: add `--first-install --factory-bank factory/bank-SMK16P1.bin`. Update omits both. Simulator: `SIM_VERSION=1.2.1 SIM_VERSION=1.2.1 sh tools/build_controls_sim.sh`. Stable and Endless MIDI A use the same runtime paths; OBV_EXPERIMENTAL only shows the experimental boot label.


## Endless MIDI 1.2.1

Official edition: Endless MIDI, identity 19653 for Pro and Elite. Includes ENDLESS per-part USB/TRS MIDI, controller bank B channel selection/layers for the keyboard, wheels and sustain, plus battery and release identification. Bank A pads and encoders/faders retain map channels. See the [current manual](MANUAL.en.md) and [release notes](RELEASE-1.2.1.en.md) for the complete workflow. ML1 /19650, ML1a /19651 and Endless MIDI A /19652 can update directly while preserving saved data.
