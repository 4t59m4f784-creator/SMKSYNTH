# Release 1.0.0 validation

[Português](VALIDATION.md) · [English](VALIDATION.en.md)

Marcus approved the DP2 runtime on his SMK‑37 Pro. Version 1.0.0 preserves it, adding factory content and the first-install profile. **Physical V016 → 1.0.0 USB installation and return to stock have not yet been confirmed.** Simulated flash does not prove real USB transport, power stability or chip CPU behaviour.

## Factory data

The source backup was verified with SHA-256 and CRCs and extracted using actual firmware types. Only patterns 1–5 and their scenes are included; 6–16 are erased. All eight selected favourites are included, including two POLY BRASS variants and INIT. No user presets are imported. Both user-preset sectors start erased and fall back to base presets.

Settings are clean: 90 BPM, neutral tuning/octave, Pattern 1, MIDI omni, internal sync. The author’s complete personal globals were not copied. Notes, sounds and mixer values are preserved; favourite generation and edited badges are normalized to the public base presets.

## Evidence

| Area | Result |
|---|---|
| Official source | Supplied `SMK-37 Pro_016(1).fwsc`, SHA-256 `2d98ce72530e71d617384963423820536f094501299e2cfec0a8d3c3fb6bcfb0`, 36 interleave markers |
| Loader | Exact final image; resumed at 154 erase and 22 program interruption points; repeat is idempotent; boot header/BT/calibration/USR/key_mac preserved |
| Package | Boot/app/cfg/EQ CRCs; seed outside boot files; 320 KiB application layout plus SMK16P1 data |
| Installer | Pinned SHA/model/CRCs; Pro V016 only; DP2 and other versions/models rejected before writing; resume bound to source/package |
| Simulator | Actual C boot; five audible examples; eight favourite recalls; eleven empty slots; 90 BPM; no automatic writes |
| Regression | Octaves and persistent transpose, split/layer and drums; ANALOG and DRUM-X PCM hashes match the base |
| Backup | 19618 client against actual C server in simulated flash: export, retry, SHA/CRC, restore and resume |

Logs: `validation/release-*.log`. Distribution hashes: `releases/1.0.0/SHA256SUMS.txt` and `RELEASE.json`.

Application: 161660/327680 bytes; static RAM: 82896/98304; pool: 325984/344064; first-install loader: 7211. The initial package replaces the application/bank area and must not be described as preserving an existing bank.

Official firmware was used locally for tests and is not redistributed. Marcus authorized the selected musical content. An isolated DP2 export-session expiry was resolved by reboot, without a confirmed root cause.

The local visual review covered landing-page navigation and the simulator’s five occupied/eleven empty pattern slots. The HTTP preview ran interface mode; AudioWorklet execution was verified in Node. Hardware audio approval applies to the DP2 base. PT/EN browser translations do not alter firmware or protocol data.
