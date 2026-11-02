# SMKSYNTH 1.0.0 installation

[Português](INSTALLATION.md) · [English](INSTALLATION.en.md)

**Model: M‑VAVE SMK‑37 Pro. Required source: official V016, identifying as `SMK-37 Pro_016`. Target: `SMK-37 Pro_19618`.** Other models, versions and alternative firmware are rejected.

Download V016 for **SMK‑37 PRO** and M‑UPGRADE from the [manufacturer](https://www.cuvave.com/appdownload). Official firmware is not redistributed. The tested V016 file has SHA-256 `2d98ce72530e71d617384963423820536f094501299e2cfec0a8d3c3fb6bcfb0`.

If you already use SMKSYNTH, save edits on the panel and export your `.smksynth` bank **before** returning to V016. Returning to official firmware and first installation can replace SMKSYNTH data. There is no bank merging.

## Install

1. Install official SMK‑37 Pro V016 with M‑UPGRADE following the manufacturer’s instructions, then reboot.
2. Use up-to-date Chrome/Edge on a computer, HTTPS, a USB data cable and stable power. Close DAWs, MIDI editors and other installer/backup tabs.
3. Open the [installer](https://4t59m4f784-creator.github.io/SMKSYNTH/install/?lang=en). It verifies SHA-256, model and CRCs before enabling installation.
4. Click **Connect and identify**. Check `SMK-37 Pro_016`.
5. Choose **Install SMKSYNTH 1.0.0**. Review the source, target and factory-bank replacement, then confirm.
6. Keep the tab, cable and power connected until reboot and the target identity `SMK-37 Pro_19618` are confirmed.
7. Pattern 1 opens stopped at 90 BPM. PLAY starts the example. KNOB BANK → Patterns provides P1–P5; P6–P16 are empty. The eight lower Performance pads recall favourites.

The package writes the application area and initializes the SMK16P1 bank. It is **not** a bank-preserving update. Boot header, BTIF, USRTRIM calibration, USR and key_mac are outside its write windows. Simulated-flash interruption tests passed; physical USB first installation and return to stock have not yet been confirmed.

## Interrupted installation

Use the same browser, website origin and package. Pending installation is bound to the package SHA-256 and V016 source. Resume is only enabled for `ota-SMK-37 Pro_900` with that binding. An unidentified loader is not automatically authorized.

Reconnect and identify again, confirm resume if offered, and wait. Download the log if it does not complete. Do not start a bank restoration while firmware installation is pending. First installation is not atomic; the loader retains responsibility for resuming until completion.

## Backup and restoration

The [backup tool](https://4t59m4f784-creator.github.io/SMKSYNTH/backup/?lang=en) exports **saved** data, including user presets, favourites, patterns/scenes and settings. Save panel edits first. Your bank files stay on your computer.

Restoration replaces the entire bank, including empty slots; it is not selective import. Export the current destination bank in the same session before confirming restoration. Type the displayed confirmation word **RESTAURAR** exactly, also in the English interface. If the instrument shows RESTORE PENDING, select the same file and finish before changing firmware. The instrument remains locked for playing until completion and reboot.

After installing 1.0.0, you can restore an earlier compatible `.smksynth` backup to recover your personal bank. This replaces the five-pattern/eight-favourite demonstration bank with your saved content. Simulator sessions are separate files and are not hardware backups.

An isolated DP2 export-session expiry was resolved by rebooting the instrument; its cause was not reproduced. An incomplete export is not presented as a verified backup.

## Return to official firmware

Export the SMKSYNTH bank first. Use M‑UPGRADE with official **SMK‑37 Pro V016**, following the manufacturer’s instructions. Do not use Elite/MKE/FM-1 files. Official firmware does not import `.smksynth` files. After reinstalling SMKSYNTH, use the backup tool to restore your work.
