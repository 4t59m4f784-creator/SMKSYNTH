#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# First build the P16 hardware/WASM as documented. No USB writes.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build validation
for test in patterns16_test save_focus_test usability_native_test save_display_test octave_test; do
 cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o "build/$test" "tests/$test.c"
 "build/$test" > "validation/p16-$test.log" 2>&1
done
cc -O1 -Ifirmware/src -Ifirmware/gen -Ifirmware/hal -o build/p16-flash-store tests/smk_voice_store_test.c
build/p16-flash-store > validation/p16-flash-store.log 2>&1
for test in octave save_display patterns16 save_focus split_layer_rows split_layer ad1_transpose drums_grid drums usability; do
 node "tests/${test}_wasm_test.mjs" > "validation/p16-$test-wasm.log" 2>&1
done
python3 tests/voice_package_bank_test.py build/obervave-3.0.1-p16o1-smk37.fwsc > validation/p16-package.log 2>&1
node tests/p16_installer_test.mjs > validation/p16-installer.log 2>&1
printf '%s\n' 'PASS: P16 full slots, flash ownership/recovery, real WASM migration/controls/audio worklet, package and installer simulation. Hardware validation pending.'
