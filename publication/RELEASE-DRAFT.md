# SMKSYNTH — first public release (draft)

**Not published. Public version/tag and factory collection pending.**

In terms of sound, SMKSYNTH brings together the excellent OberVave synthesizer and the DRUM-X engine from the equally excellent JIANT. But an instrument is more than its sound. The idea behind this project is to turn the M-VAVE SMK-37 into an instrument built for performance, adding features and rethinking an experience that makes use of its pads and controls. The pads are the heart of SMKSYNTH. With this project, I set out to implement my vision of an ideal performance instrument in this compact, 37-key format.

## Planned release assets

- Firmware `.fwsc` for the verified target and update/first-install paths.
- Complete corresponding source archive from the tagged commit, including build and install scripts, licenses and notices.
- SHA256SUMS and a machine-readable manifest tying the firmware to the exact source commit.
- Selected factory collection, after integration and validation.

## Upgrade and data

Back up before changing firmware. Full restore replaces the saved bank and requires explicit confirmation. B1/B2 backups use the existing versioned format. The new-install factory collection must not overwrite an existing user's bank.

## Validation to report at release time

Separate automated checks, physical playback/USB checks, first-install checks and interruption tests. Record the exact shipped build; do not reuse historical test claims for a changed binary.

## Credits

SMKSYNTH by Marcus Padrini (credit pending approval), derived from OberVave by zvenson, with Felucca/Dinghy, SLOOP, SMK-37 port and JIANT/DRUM-X contributions. See CREDITS.md and LICENSING.md. GPL-3.0-only with separately identified components.
