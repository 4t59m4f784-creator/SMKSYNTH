#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
mkdir -p build/host validation
VD1_PACKAGE=${VD1_PACKAGE:-build/obervave-3.0.1-vd1-smk37.fwsc}
cc -O1 -Ifirmware/hal -Ifirmware/src -o build/host/smk_voice_store_test tests/smk_voice_store_test.c
build/host/smk_voice_store_test > validation/vd1-store.log
python3 tests/smk_pkg_test.py "$VD1_PACKAGE" 'SMK-37 Pro_19601' > validation/vd1-package.log
python3 tests/voice_package_bank_test.py "$VD1_PACKAGE" > validation/vd1-boot-bank.log
node tests/vd1_installer_test.mjs > validation/vd1-installer.log
node tests/vd1_controller_test.mjs > validation/vd1-controller.log
cc -O1 -DBOARD_SMK37=1 -Ifirmware/hal -Ifirmware/src -o build/host/vd1-ota-test tests/ota_test.c
build/host/vd1-ota-test "$VD1_PACKAGE" > validation/vd1-ota.log
# Return packages are separately included in the distribution, never regenerated.
PF12_PACKAGE=${PF12_PACKAGE:-web/voice-lab/keyboard/firmware/obervave-3.0.1-pf12-smk37.fwsc}
PF9_PACKAGE=${PF9_PACKAGE:-web/voice-lab/keyboard/firmware/obervave-3.0.1-pf9-smk37.fwsc}
cc -O1 -DBOARD_SMK37=1 -Ifirmware/hal -Ifirmware/src -o build/host/vd1-loader-test tests/ldr_test.c
build/host/vd1-loader-test "$PF12_PACKAGE" "$VD1_PACKAGE" > validation/vd1-loader.log
build/host/vd1-loader-test "$VD1_PACKAGE" "$PF12_PACKAGE" > validation/vd1-pf12-return.log
build/host/vd1-loader-test "$VD1_PACKAGE" "$PF9_PACKAGE" > validation/vd1-pf9-return.log
printf '%s\n' 'PASS: VD1 package, private NOR store, boot CRC isolation, installer/controller, OTA and PF12/PF9 returns. Hardware not tested.'
