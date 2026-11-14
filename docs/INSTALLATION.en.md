# SMKSYNTH 1.2.0 - Installation and recovery

The same [installer](../install/?lang=en) supports SMK-37 Pro and SMK-37 Elite. It identifies the model and selects a specific package automatically. Use desktop Chrome/Edge over HTTPS, a USB data cable and stable power; close other MIDI apps.

1. Connect and identify. The keyboard is not accessed until you click Connect.
2. From official V016 of the connected model, the page offers first installation. This initializes/replaces the bank with five patterns and eight favourites.
3. From supported SMKSYNTH builds, the page offers Update, preserving saved programs, favourites, patterns and settings. Save changes and export a [backup](../backup/?lang=en) before updating.
4. Check the detected model and operation, confirm and keep the connection until target identity `SMK-37 Pro_19649` or `SMK-37 Elite_19649` is confirmed.
5. Connect the sustain pedal before restarting and keep it released during startup.

## Interrupted transfers

Resume on the same page/browser. The pending operation is bound to model, original source, package SHA and operation type. A Pro loader cannot accept an Elite package or vice versa. An unknown loader without a matching pending operation is refused.

For an older interrupted operation, use the preserved [Pro 1.0.3 installer](../install/legacy-1-0-3/) or [Elite2 installer](../install-elite/legacy-elite2/). Their original packages and pending keys are retained. Earlier Pro links remain on the main installer. Do not switch versions during a pending restore/update.

## Returning to stock

Export a backup before returning to M-VAVE firmware. Use the manufacturer's M-UPGRADE with the exact official V016 package for your model. The preserved Elite2 page also retains its verified Elite V016 return flow. Returning to stock can replace SMKSYNTH data. Reinstalling SMKSYNTH is a first installation; restore the saved backup afterward.

## After installation

See the [manual](MANUAL.en.md) for USB/TRS routing, physical sustain and GLOBE + PLAY Panic. ENDLESS remains a separate line; this release preserves saved-bank bytes on update but does not add ENDLESS features. Restoring the prior backup is required to undo edits made in another line.
