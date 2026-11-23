# Public validation 1.3.0

Approved runtime: SU3 /19663; firmware sources are byte-identical. Public identity 19664, Pro and Elite. Only build version/identity and first-install seed differ. Native RAM 96,352/98,304 bytes; pool 332,180/344,064, voice 184 bytes. No poliphony reduction.

Four packages verified: SHA-256, CRC, model/loader identity, matching initial/update applications and factory seed only in initial install. Automated native SU3, sanitizer, additive SUB, dual-core fallback/audio, controls, MIDI and WASM regressions pass. Backup supports public/experimental identities, including 19663 and 19664. Host tests do not replace hardware installation or CPU measurements. The user approved SU3; each final public package has not been separately flashed on physical hardware. Known iPhone USB-video clicks remain.

[Release](RELEASE-1.3.0.en.md) · [Manifest](../RELEASE.json)

Installation checks distinguish first installation from an update: only the matching official V016 model can receive the factory-seeded package; existing SMKSYNTH identities use the preserving update. Wrong-model packages, altered hashes and CRCs, substituted devices, foreign loaders and invalid pending journals are rejected before writes. Interruption/resume tests preserve the original source identity. The public 1.2.2 installer remains archived for its unfinished transfers. The WebAssembly simulator runs the same musical/UI sources with a serial host renderer; it demonstrates controls and sound but does not measure the hardware's dual-core headroom.
