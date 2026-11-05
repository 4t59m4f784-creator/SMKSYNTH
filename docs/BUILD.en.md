# Building SMKSYNTH 1.0.3

[Português](BUILD.md) · [English](BUILD.en.md)

Identity: `SMK-37 Pro_19618`. Runtime base: approved SUPER SS3 commit `0b4ccdd4892dd6d6d5a1a4f206aa939aca46a853`. Synthesis, sequencing, interface and data formats are preserved from that base.

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
  --release 1.0.3 --identity 19618 --first-install \
  --factory-bank factory/bank-SMK16P1.bin --sdk /path/to/SDK
```

Output: `build/obervave-1.0.3-smk37.fwsc`; distribution name: `SMKSYNTH-1.0.3-SMK37-Pro.fwsc`.

The first-install profile uses the platform’s standard full-area loader to initialize `0x4000..0x9CFFF`, including the SMK16P1 seed. The application retains SMK16P1 storage. The browser requires official V016; this is not the SUPER SS3 bank-preserving updater.

The restore journal starts erased at `0x55000..0x56FFF`; the seed occupies `0x57000..0x9CFFF`. No boot-file CRC includes this data area. Boot header, calibration and protected manufacturer regions are not written. The loader’s original sector comparison/verification and resume flow remain in use.

## Simulator and web distribution

```sh
sh tools/build_controls_sim.sh
cp build/controls/SMKSYNTH-1.0.3.wasm web/simulator/obervave-voice.wasm
node tests/release_demo_wasm_test.mjs
node tests/release_101_installer_test.mjs
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
