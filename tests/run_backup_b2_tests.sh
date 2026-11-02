#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
b1=${1:?Provide exact B1 firmware package}
mkdir -p build validation
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/backup_fixture tests/backup_fixture.c
build/backup_fixture
python3 tests/backup_extract_test.py > validation/b2-extract.log
cc -O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all -o build/backup_protocol tests/backup_protocol_test.c
ASAN_OPTIONS=detect_leaks=0 build/backup_protocol > validation/b2-legacy-protocol.log
node tests/backup_client_test.mjs > validation/b2-legacy-client.log
cc -O2 -g -fsanitize=address,undefined -fno-sanitize-recover=all -o build/backup_restore_test tests/backup_restore_test.c
ASAN_OPTIONS=detect_leaks=0 build/backup_restore_test build/b2-fixture-bank.bin > validation/b2-restore.log
cc -O2 -o build/backup_server tests/backup_server.c
node tests/backup_b2_client_test.mjs > validation/b2-client.log
node tests/backup_b2_installer_test.mjs > validation/b2-installer.log
cc -O2 -DBOARD_SMK37=1 -DOBV_VOICE_HW=1 -o build/backup_loader tests/backup_loader_test.c
build/backup_loader "$b1" build/obervave-3.0.1-b2-smk37.fwsc > validation/b2-loader.log
cc -O2 -DBOARD_SMK37=1 -o build/backup_ota tests/ota_test.c
build/backup_ota build/obervave-3.0.1-b2-smk37.fwsc > validation/b2-ota.log
python3 tests/voice_package_bank_test.py build/obervave-3.0.1-b2-smk37.fwsc > validation/b2-package.log
printf '%s\n' 'PASS: B2 automated suite. Hardware/USB speed and interruption tests remain pending.'
