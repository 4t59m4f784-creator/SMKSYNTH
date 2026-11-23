# Public validation 1.3.1

Approved runtime: PS3 /19667. Firmware sources are byte-identical to the approved PS3; promotion changes build version/identity to 19668 for Pro and Elite. Static RAM 96,384/98,304 bytes; pool 332,180/344,064; unchanged polyphony. No additional synthesis changes were introduced in the promotion.

The user confirmed the first kick when entering pattern 1 and strong stability during repeated recalls with different tempos. Development checks covered PS1/PS2/PS3 native regressions, sanitizers, first attack in the recall CTL, 10,128 rapid requests and 253 commits, same-engine recall PCM identical to PS2, dual-core audio/fallback, controls, MIDI and WASM.

All four final public packages are checked for SHA-256, CRC, model and loader identities. Initial-install/update applications match; only first installation seeds the unchanged factory bank. Updating preserves bank, theme and MIDI maps. Backup accepts identities 19664–19668 and earlier compatible versions.

Host tests do not replace physical CPU measurement or installation of each package on each model. PS3 runtime was hardware-approved; all four final 1.3.1 packages have not been individually installed. Known iPhone USB-video clicks remain. The 1.3.0 manual/PDFs still apply because no features changed.

[Release](RELEASE-1.3.1.en.md) · [Manifest](../RELEASE.json)
