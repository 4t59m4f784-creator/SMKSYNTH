# Backup and restore

Open `backup-b2/` on the website using Chrome or Edge on a computer. Connect the instrument with a USB data cable and authorize MIDI/SysEx when requested.

## Export

Save presets, favorites and patterns/cenes using their normal controls first. Stop PLAY/REC and release keys, buttons and sustain. Close other MIDI apps. Export the saved bank and keep the `.smksynth` file on your computer.

The complete bank includes user preset records, eight favorites, 16 pattern positions with scenes/sounds/mix and saved settings. Empty positions are included. Edits not saved on the instrument are excluded.

B2 uses 512-byte blocks and compression when useful, then independent sector CRC checks and a computer-side SHA-256. B1 files remain compatible. The B1 method uses two complete reads. Marcus confirmed that B2 export and restore work on the SMK-37 and are substantially faster; no exact hardware duration was recorded.

## Restore

A restore **replaces the entire bank**, including saved settings. It does not merge collections or selectively import presets. Export the current device bank in the same browser session first; the tool requires this protection before starting a new restore.

Choose the desired `.smksynth`, review its summary, type RESTAURAR and confirm. Keep power/cable/tab connected. After verification, use the tool's restart button to reload the restored state.

B2 restores one prepared/verified sector at a time. It is not atomic across the entire bank. During restore the musical engine is silenced and the instrument stays locked until verification and restart.

## Resume

If interrupted, reconnect and select the **same original file**. The pending operation is tied to that bank's hash. Do not change firmware to try to clear it. There is no automatic whole-bank undo: keep prior backups.

## Simulator sessions

Simulator-session JSON and device `.smksynth` are different formats and storage systems. This release does not convert one to the other.

Files are processed by the browser and device; the backup application does not upload bank contents to a server. Your download folder is the place to retain your copies.
