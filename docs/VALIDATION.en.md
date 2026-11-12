# Public release 1.1.0 validation

- Marcus approved all MIDI M1.2 base features on SMK-37 Pro: TRS/USB MIDI, part-specific pitch bend/mod wheel, sustain pedal, MIDI PANIC and stuck-note fix. An actual user validated the SMK-37 Elite installation workflow on hardware.
- Final 1.1.0 packages were rebuilt from that tested base with identity 19634 on both models. The hardware approval above refers to the tested base and Elite workflow, rather than a fresh physical installation of these four final files.
- Application: 167200 bytes; static RAM: 94480/98304 bytes; pool: 325984/344064 bytes; loader: 7703 bytes.
- Unified installer: four distinct model/operation packages, SHA-256 and CRC checks, automatic selection, refusal of model/device substitution, unknown origins and foreign loaders. Resume is bound to the model, package and initial source. First installation seeds the factory bank; updating preserves the bank.
- Host regression covers part note routing, CC1/pitch bend/CC64, all-channel Panic, note release after mode changes, sustain with other held keys, split/layer and sequencer, USB, backup, simulator and PT/EN. Elite flash tests include interruption/resume and bank preservation.
- All four final packages are inspected for model, identities, loader, application and factory data. Publication checks public documentation and links.
- ENDLESS remains separate. USB audio retains the known iPhone video-recording limitation. Host tests do not measure real CPU usage or replace hardware testing.

Hashes: [RELEASE.json](../RELEASE.json). Download checksums: `releases/1.1.0/SHA256SUMS.txt`.

## Historical 1.0.3 record

# Public release 1.0.3 validation

The public runtime promotes SUPER SS3 (`0b4ccdd4892dd6d6d5a1a4f206aa939aca46a853`), approved by Marcus on the SMK-37 Pro after chord tests.

Public target build: application 164052 bytes; static RAM 86624/98304 bytes; pool 325984/344064 bytes; preserving-update loader 7703 bytes. RAM-critical code has no outbound calls.

DSP checks cover 32768 SUPER blocks, 1572840 exact reciprocal divisions and 32768 companion SAW blocks against reference results. The mixed-optimization integration fixture covers chords, high notes, release overlap, modulation, dual SUPER and effects. Its PCM SHA-256 is `7c7688bb6186498d7f9abe83db713e8ef7f4c98b69bfe094007965aa50471fba`.

Simulated-flash loader tests exercise 1.0.2 → 1.0.3 and SS3 → 1.0.3 updates, preserving the saved bank byte for byte, protected regions and restore journal, including 42 erase interruption/retry points. First installation uses a separate factory-seeded package.

Installer checks validate pinned hashes, identities, model refusal, same-identity updates, experimental SUPER origins and interrupted resumes bound to package/source. Historical packages and installer pages remain available.

The public WebAssembly simulator is rebuilt with SUPER. Real firmware boot checks verify five patterns, eight favourites, 90 BPM, empty slots 6–16 and no automatic writes. MIDI and physical controls are covered by regression tests.

USB audio keeps the UA1/1.0.2 implementation, tested on Mac and on iPhone with AudioShare. iPhone USB video recording clicks/dropouts remain a known issue.

Package, WASM and factory-content hashes are in [RELEASE.json](../RELEASE.json); download checksums are in `releases/1.0.3/SHA256SUMS.txt`. Supersaw origin: [Clipper](https://github.com/isod89/clipper-fm1).

Host and simulated-flash checks do not measure physical CPU time. Hardware sound/performance approval is Marcus’s SS3 test; it does not guarantee every unison/effects/polyphony combination.
