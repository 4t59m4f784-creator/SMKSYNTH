# SMKSYNTH

**A performance instrument for the M‑VAVE SMK‑37 Pro.**

Its sound comes from OberVave and JIANT's DRUM-X engine. SMKSYNTH focuses on the instrument around them: persistent pad banks, instant favourite sounds, four parts, split/layer configurations, sequencing and scene recall on a compact 37-key keyboard.

## MIDI keyboard input

The simulator accepts MIDI input with velocity, sustain, pitch bend and modulation. Start audio, click **Connect MIDI keyboard**, and choose an input. Use a Web MIDI browser such as desktop Chrome or Edge. [Connection and playing guide](SIMULATOR-MIDI.md#english).

## Performance features demonstration

[Watch a brief SMKSYNTH demonstration on YouTube](https://www.youtube.com/watch?v=Ax-RqjtUFjc).

## AI-assisted development

SMKSYNTH was developed using AI. Marcus Padrini defined the goals and performance workflow, directed the work, selected the sounds, created the five patterns, and performed hardware testing and approval. ChatGPT, by OpenAI, contributed technical proposals and solutions and generated and revised code for the adaptations, web tools, documentation and graphics under his direction. The underlying engines and components are the work of the authors credited below. This independent project is not affiliated with or endorsed by OpenAI.

Factory setup: five patterns by Marcus Padrini, eight favourites, empty slots 6–16 and a 90 BPM starting tempo. The base preset bank is unchanged. Edited sounds live in scene/favourite snapshots; no extra user presets are installed.

**First installation requires official M‑VAVE V016 on SMK‑37 Pro, identifying as `SMK-37 Pro_016`.** Other models/versions are rejected. First installation initializes the bank. Existing SMKSYNTH 1.0.x and SUPER SS1/SS2/SS3 users can choose Update to preserve their saved bank. Export work before returning to stock. Official M‑VAVE firmware is not redistributed.

[Website](https://4t59m4f784-creator.github.io/SMKSYNTH/?lang=en) · [Simulator](https://4t59m4f784-creator.github.io/SMKSYNTH/simulator/?lang=en) · [Installer](https://4t59m4f784-creator.github.io/SMKSYNTH/install/?lang=en) · [Backup](https://4t59m4f784-creator.github.io/SMKSYNTH/backup/?lang=en)

The SUPER SS3 runtime was approved by Marcus on his keyboard. The 1.0.3 initial-install package passed simulated-flash tests using the supplied V016 image, including interruption/resume. Marcus subsequently confirmed physical USB first installation on his instrument. The public tools include a PT / EN language selector.

OberVave/Dinghy: zvenson. JIANT/DRUM-X: Juan Ignacio Antonucci and contributors. Felucca: Leo Kuroshita/Hügelton Instruments. SLOOP: isod89. SMK port/tools: M. Geisler/genkigenki and contributors. SMKSYNTH direction, examples and hardware validation: Marcus Padrini. Technical proposals, adaptation code, tools and documentation: developed with ChatGPT / OpenAI under his direction. GPL-3.0-only; [LICENSE](../LICENSE), [CREDITS](../CREDITS.md), [LICENSING](../LICENSING.md).

[Installation and recovery](INSTALLATION.en.md) · [User guide](USER_GUIDE.en.md) · [Build](BUILD.en.md) · [Validation](VALIDATION.en.md) · [Downloads](https://github.com/4t59m4f784-creator/SMKSYNTH/releases/tag/v1.0.3)

## Documentation

| Document | English | Português |
|---|---|---|
| Quick start | [Read](QUICKSTART.en.md) · [PDF](pdf/SMKSYNTH-1.0.3-Quick-Start-EN.pdf) | [Ler](QUICKSTART.md) · [PDF](pdf/SMKSYNTH-1.0.3-Quick-Start-PT.pdf) |
| Complete manual | [Read](MANUAL.en.md) · [PDF](pdf/SMKSYNTH-1.0.3-Manual-EN.pdf) | [Ler](MANUAL.md) · [PDF](pdf/SMKSYNTH-1.0.3-Manual-PT.pdf) |

Verified pad, button, knob and fader maps, recording, scenes, effects, MIDI, saving and backup.

## SUPER / Supersaw

SUPER adds seven saws per oscillator, independent DETUNE and MIX, and a dedicated PARA page (DET1 / MIX1 / DET2 / MIX2). PW becomes DETUNE when WAVE is SUPER. Settings are saved with presets, favourites, scenes and backups. Eight ANALOG voices remain shared. SYNC and X-MOD are bypassed with SUPER; UNISON consumes whole voices. See the manual for controls and limits.

The supersaw originates in **[Clipper by 3dSam](https://github.com/isod89/clipper-fm1)**, built on Felucca and citing Adam Szabo’s analysis. SMKSYNTH adapts its detune curve and offsets with independent MIX and its own optimizations. Original GPL-3.0-only notices and attribution are preserved.

Stereo USB audio is retained (16-bit, 44.1/48 kHz), tested on Mac and iPhone with AudioShare. Known issue: iPhone video recordings using USB audio may contain clicks/dropouts.
