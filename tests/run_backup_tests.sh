#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Run after building B1; provide the original P16.2 .fwsc as argument 1.
set -eu
cd "$(dirname "$0")/.."
stable=${1:?Provide original P16.2 firmware package}
test -f "$stable"
mkdir -p build/host validation
cc -O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all -o build/host/backup_protocol tests/backup_protocol_test.c
# LeakSanitizer cannot inspect /proc in the managed runtime; retain ASan/UBSan.
ASAN_OPTIONS=detect_leaks=0 build/host/backup_protocol > validation/b1-protocol.log
node tests/backup_client_test.mjs > validation/b1-client.log
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/backup_fixture tests/backup_fixture.c
build/host/backup_fixture
python3 tests/backup_extract_test.py > validation/b1-extract.log
cc -O1 -DBOARD_SMK37=1 -DOBV_VOICE_HW=1 -o build/host/backup_loader tests/backup_loader_test.c
build/host/backup_loader "$stable" build/obervave-3.0.1-b1-smk37.fwsc > validation/b1-preserve.log
node tests/backup_installer_test.mjs > validation/b1-installer.log
printf '%s\n' 'PASS: B1 automated suite. Physical USB/hardware validation remains pending.'
