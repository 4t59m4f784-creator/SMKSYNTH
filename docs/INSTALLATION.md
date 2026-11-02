# Installation and updating

## Supported today

The included B2 installer accepts **SMK-37 Pro_19613, _19614 or _19615**. Target: **_19615**. It checks the package hash and device identity before writing. It does not provide a first installation from stock firmware, another developer's firmware, FM-1, Elite or MKE-P37. Do not bypass these checks.

The first-install route is a **public-launch prerequisite**, to be prepared and tested separately. Until then, the review website labels this tool as an update for existing SMKSYNTH installations.

## Update an existing supported installation

1. Export a `.smksynth` backup using the current firmware's compatible tool. Keep the file on your computer.
2. Use Chrome or Edge on a computer, USB data cable and stable power. Close other MIDI apps and tabs.
3. Open the installer over HTTPS (or localhost for development), connect and read the detected identity.
4. Review the destination build and confirm explicitly. Keep the cable, power and tab connected until identity 19615 is confirmed.
5. Reconnect, verify your saved sounds and export a new backup.

B2 preserves a recognized SMK16P1 bank. This path has automated interruption tests, and Marcus reported successful device export/restore with B2. Physical power-cut testing has not been reported; do not equate simulated interruption tests with physical certification.

## Interrupted update or restore

An interrupted firmware update and an interrupted bank restore use different recovery paths. Follow the page that started the operation. The B2 installer can resume its own recognized interrupted update; it is not a general rescue tool.

If the instrument displays **RESTORE PENDING / RESUME VIA USB**, use the backup tool with the **same bank file**. Do not install another firmware or use an old P16.2/B1 installer while restoration is pending. See [BACKUP.md](BACKUP.md).

Public instructions for return to stock/other firmware remain pending validation. The review does not include an unverified recovery binary.
