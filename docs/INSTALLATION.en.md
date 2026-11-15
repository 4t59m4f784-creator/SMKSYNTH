# SMKSYNTH 1.2.1 - Installation and recovery

The same [installer](../install/?lang=en) supports SMK-37 Pro and SMK-37 Elite. It identifies the model and selects a specific package automatically. Use desktop Chrome/Edge over HTTPS, a USB data cable and stable power; close other MIDI apps.


## Preparation and precautions

- **First installation:** use official V016 for your exact model. If it runs another M-VAVE version, use [M-UPGRADE for Windows](https://yms-file-store.oss-cn-hongkong.aliyuncs.com/software/pc/M-UPGRADE.zip) or [macOS](https://yms-file-store.oss-cn-hongkong.aliyuncs.com/software/pc/M-UPGRADE.dmg), with [SMK-37 Pro V016](https://yms-file-store.oss-cn-hongkong.aliyuncs.com/software/firmware/SMK-37Pro/SMK-37%20Pro_016.fwsc) or [SMK-37 Elite V016](https://yms-file-store.oss-cn-hongkong.aliyuncs.com/software/firmware/SMK-37Elite/SMK-37%20Elite_016.fwsc). Never swap files between models. If the correct V016 is already installed, skip this step.
- **Already using SMKSYNTH:** update directly in the installer. Returning to V016 is unnecessary and may erase your bank. Save your edits and export a backup first.
- **While writing:** never remove USB or power off the keyboard. Do not switch tabs, close or reload the page, or allow the computer to sleep. Wait for Complete and final identity confirmation. A pause at the Loader stage does not mean writing has finished.


1. Connect and identify. The keyboard is not accessed until you click Connect.
2. From official V016 of the connected model, the page offers first installation. This initializes/replaces the bank with five patterns and eight favourites.
3. From supported SMKSYNTH builds, the page offers Update, preserving saved programs, favourites, patterns and settings. Save changes and export a [backup](../backup/?lang=en) before updating.
4. Check the detected model and operation, confirm and keep the connection until target identity `SMK-37 Pro_19653` or `SMK-37 Elite_19653` is confirmed.
5. Connect the sustain pedal before restarting and keep it released during startup.

## Interrupted transfers

Resume on the same page/browser. The pending operation is bound to model, original source, package SHA and operation type. A Pro loader cannot accept an Elite package or vice versa. An unknown loader without a matching pending operation is refused.

For an older interrupted operation, use the preserved [Pro 1.0.3 installer](../install/legacy-1-0-3/) or [Elite2 installer](../install-elite/legacy-elite2/). Their original packages and pending keys are retained. Earlier Pro links remain on the main installer. Do not switch versions during a pending restore/update.

## USB RESCUE

After a failed update, only when no writing is in progress, power off, hold only OCTAVE − and power on to try USB RESCUE. Use M-UPGRADE with your exact model’s V016 to return to stock; this may erase SMKSYNTH data. If USB is not recognized, keep the log and seek guidance rather than forcing another model’s file.

## Returning to stock

Export a backup before returning to M-VAVE firmware. Use the manufacturer's M-UPGRADE with the exact official V016 package for your model. The preserved Elite2 page also retains its verified Elite V016 return flow. Returning to stock can replace SMKSYNTH data. Reinstalling SMKSYNTH is a first installation; restore the saved backup afterward.

## After installation

See the [manual](MANUAL.en.md) for USB/TRS routing, physical sustain and GLOBE + PLAY Panic. Version 1.2.1 includes ENDLESS and MIDI MODE. Update preserves the saved bank and compatible separate theme/map records. A .smksynth backup contains the synth bank, not MIDI maps or theme. Restore the prior backup to undo synth-bank edits made in another line.


## Endless MIDI 1.2.1

Official edition: Endless MIDI, identity 19653 for Pro and Elite. Includes ENDLESS per-part USB/TRS MIDI, controller bank B channel selection/layers for the keyboard, wheels and sustain, plus battery and release identification. Bank A pads and encoders/faders retain map channels. See the [current manual](MANUAL.en.md) and [release notes](RELEASE-1.2.1.en.md) for the complete workflow. ML1 /19650, ML1a /19651 and Endless MIDI A /19652 can update directly while preserving saved data.
