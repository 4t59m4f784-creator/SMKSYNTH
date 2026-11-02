# SMKSYNTH

**A performance synthesizer for the M-VAVE SMK-37 Pro.**

[Português](README.pt-BR.md) · [Installation](docs/INSTALLATION.md) · [Backup & restore](docs/BACKUP.md) · [Build](docs/BUILD.md) · [Credits](CREDITS.md)

In terms of sound, SMKSYNTH brings together the excellent [OberVave](https://github.com/zvenson/obervave) synthesizer and the DRUM-X engine from the equally excellent [JIANT](https://github.com/juanjiant-bit/JIANT).

But an instrument is more than its sound. The idea behind this project is to turn the M-VAVE SMK-37 into an instrument built for performance, adding features and rethinking an experience that makes use of its pads and controls.

Switch easily between parts, save favorite sounds and recall them with a single tap, record sequences, create layers and splits, and save scenes to recall your settings during a performance.

The pads are the heart of SMKSYNTH. With this project, I set out to implement my vision of an ideal performance instrument in this compact, 37-key format.

> **Publication review — not a public release.** This package uses B2 (`SMK-37 Pro_19615`), whose export and restore have been tested on Marcus's physical keyboard. The public factory collection is being selected. First installation from stock firmware is not yet supported by the included B2 installer. No public repository, release or Pages address has been created by this package.

## Pad bank guides

[Performance & favorites](docs/pad-banks/01-performance-favoritos.png) · [SPLIT / LAYER](docs/pad-banks/02-split-layer.png) · [Scenes & patterns](docs/pad-banks/03-cenas-patterns.png)

The illustrated guides are in Portuguese; editable SVG files are in `docs/pad-banks/`.

## Play, program, perform

| Feature | Current base |
| --- | --- |
| Parts | Four, A–D |
| ANALOG | Eight melodic voices shared across parts; two oscillators, filters, envelopes, LFO and arpeggiator |
| DRUM-X | Eight synthesized drum instruments; five kits; no third-party samples |
| Sequencing | Four tracks, up to 256 steps per track, automation and synchronized pattern changes |
| Patterns | 16 saved positions with sounds, mix and scenes |
| Presets & favorites | 99 program positions and eight favorites; manual saving |
| Keyboard | Per-part octave; SINGLE, SPLIT and LAYER, with two saved keyboard configurations |
| Effects | Chorus, delay and reverb sends; drive and master processing |
| Backup | USB-MIDI export and complete restore in a browser; `.smksynth` files |
| Simulator | Browser performance simulator of the approved P16.2 musical base |

**Save deliberately.** BT saves presets. Patterns are saved through the pattern pads, with a separate overwrite confirmation. PLAY and REC preserve the selected pad bank. See [the quick guide](docs/QUICKSTART.md).

## Browser tools

The `site/` folder contains a self-contained static website with three entry points:

- **Installer:** updates existing SMKSYNTH identities 19613/19614/19615 to B2/19615. It refuses unsupported origins.
- **Backup / restore:** exports saved device data, restores a complete bank and resumes an interrupted restore with the same file.
- **Simulator:** plays the P16.2 musical base without installing anything. Its browser-session files are separate from hardware `.smksynth` backups; it does not simulate B2 USB transfers.

The website is prepared for a GitHub project subpath. See [publication instructions](publication/REVIEW.pt-BR.md). The included manual Pages workflow does not publish on push.

## Releases and source

B2 firmware SHA-256: `50054674cc3db57c971321b9f94993099f1a5505125917dd23400588914a5685`.

This review keeps the tested firmware unchanged. Source, build/install scripts, component notices and validation evidence are included. Public version naming, tag, first-install path and factory collection are not finalized. Do not rename a newly compiled binary as this tested build without new validation.

[Build instructions](docs/BUILD.md) · [Compatibility](docs/COMPATIBILITY.md) · [Changelog](CHANGELOG.md) · [Contributing](CONTRIBUTING.md).

## Credits and license

SMKSYNTH project direction and performance workflow: **Marcus Padrini** (proposed credit for review). Original copyrights remain with their respective authors. See [CREDITS.md](CREDITS.md), [LICENSING.md](LICENSING.md) and the complete [GPL version 3](LICENSE). Code is GPL-3.0-only except separately identified components. No Mellotron recordings or SMKTRON samples are included. This is an independent community project; it is not an official M-VAVE release.
