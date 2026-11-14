# Public 1.2.0 validation

- Marcus approved MO1a /19648 on Pro hardware, including ENDLESS, MIDI MODE, Zoom, preset naming and colours. A real user previously validated the Elite installation flow. Promotion changes version/identity without changing approved behaviour.
- Stable identities: SMK-37 Pro_19649 and SMK-37 Elite_19649. Pro application 224880 bytes; RAM 95392/98304; pool 328368/344064; loader 7703 bytes. These are build limits, not real-time audio-load measurements.
- All four final packages have SHA-256, CRC, model, application/loader identity and seed checks. First installation contains the factory bank; Update retains bank bytes. Simulated 1.1.0 → 1.2.0 updates passed 56 interruption/resume points per model, including protected areas and journal handling.
- Native regressions cover per-part MIDI, ENDLESS, sustain/Panic, controller transitions, output backpressure, long SUPER tails with five-voice allocation, colours, presets, sequencing, pads, Split/Layer and physical controls.
- Public WASM executes the real firmware loop for ENDLESS (3/5/7 lengths, fixed C4, HOLD, drums and saves), independent MIDI keys/pads/controls, synth return, persistent theme and separate preset confirmation. The demo retains five patterns, eight favourites, 90 BPM and no extra personal presets.
- Website, installer and simulator retain PT/EN; links, four pins, corresponding source and four versioned PDFs are checked. The hero captures the actual firmware splash renderer.
- Each of the four final 1.2.0 packages has not had a new physical installation. Host/simulated-flash tests do not replace hardware CPU/audio testing. Known iPhone USB-video clicks/dropouts remain.

Hashes: [RELEASE.json](../RELEASE.json). Downloads: `releases/1.2.0/SHA256SUMS.txt`.

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
