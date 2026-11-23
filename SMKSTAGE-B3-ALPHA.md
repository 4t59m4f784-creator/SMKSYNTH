# SMKSTAGE — B3 Hardware Alpha (development branch)

**NOT INSTALLABLE / NÃO INSTALAR** — This branch is an engineering checkpoint, not an approved firmware or release. The CI workflow compiles and checks bounds, but intentionally uploads no flashable package.

## Sources

- Stage-specific SMK-37 input, screen, pads, audio, MIDI: `firmware/src/smkstage.c` (new GPL-3.0-only work)
- SMK-37 hardware platform and flash protections: base SMKSYNTH checkout (GPL-3.0-only).
- B3 fixed-point organ: R-Jeb/setBfree-fm1 at `4dd709b18b24cb5934dd57796ac65940722d9383` (GPL-3.0-only), derived from pantherb/setBfree (GPL-2.0-or-later). Imported `organ_{core,tab,fx,od,contrib,leslie,pgm}.h`.
- The only modification to the imported audio code is rotary-only bypass inside `organ_core.h`, keeping reverb active when Pad 9 disables rotary.

## Hardware features wired in the first bring-up

- 37 keybed notes, 3 manuals, correct Note-Off manual when switching.
- K1..K8 for drawbars; F1 for ninth (pickup); F2 swell, F3 drive, F4 reverb.
- Pads 9..16 for Leslie, percussion, V/C and overdrive; Leslie Pad 10 quick Slow/Fast and long Stop.
- 8 favorites per bank **RAM only**; bank A drawbars, bank B sound snapshot (advanced settings are not fully captured yet).
- Simplified 240x240 graphic showing drawbars, manual and editing pages; pads RGB feedback.
- USB MIDI input; two-core tonewheel dispatch through the SMKSYNTH multicore driver.
- Stage development disables the SMKSYNTH auto-clear store-owner routine so it will not erase another firmware's saved data at startup.

## Explicit limitations before on-device installation

1. First-time flash map / update loader / recovery audit with the exact SMK-37 Pro unit.
2. Toolchain compilation and linker memory bounds (see Actions branch workflow).
3. Real-device audio under maximum drawbars + chorus + Leslie + drive + reverb; verify two-core stability.
4. Proper preset persistence with versioned namespace and wear-limited explicit writes. Current presets live only in RAM.
5. Acceleration-synchronous Leslie LED pulse. Current pulse is a *mode-based approximation*, not rotor-speed feedback.
6. Full screens for advanced EQ, full Leslie parameters, preset naming, boot loading and battery display.
7. Read-only device identity detection and user-specific installer approval; no public install link yet.
8. Standalone SMKSTAGE repository still to be created: this isolated branch does **not** replace SMKSYNTH main.

Source and credits must accompany every distributed firmware under the GPL obligations.
