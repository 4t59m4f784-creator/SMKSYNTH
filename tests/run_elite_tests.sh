#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/elite
stock=build/elite/SMK-37-Elite_016.fwsc
if ! test -f "$stock"; then
 curl -fL --retry 2 'https://yms-file-store.oss-cn-hongkong.aliyuncs.com/software/firmware/SMK-37Elite/SMK-37%20Elite_016.fwsc' -o "$stock"
fi
node tests/elite_installer_test.mjs "$stock"
python3 tests/elite_package_test.py
python3 tests/release_package_test.py web/install-elite/firmware/SMKSYNTH-1.0.3-elite2-first-install.fwsc
python3 tests/smk_pkg_test.py web/install-elite/firmware/SMKSYNTH-1.0.3-elite2-update.fwsc 'SMK-37 Elite_19629'
cc -O1 -DBOARD_SMK37=1 -o build/elite/first-loader tests/release_loader_test.c
build/elite/first-loader "$stock" web/install-elite/firmware/SMKSYNTH-1.0.3-elite2-first-install.fwsc
cc -O1 -DBOARD_SMK37=1 -DOBV_VOICE_HW=1 -o build/elite/update-loader tests/backup_loader_test.c
build/elite/update-loader experimental/elite1/SMKSYNTH-1.0.2-elite1-SMK37-Elite.fwsc web/install-elite/firmware/SMKSYNTH-1.0.3-elite2-update.fwsc
cc -O1 -DBOARD_SMK37=1 -DT_MODEL=1 -o build/elite/stock-return tests/elite_stock_return_test.c
build/elite/stock-return "$stock"
