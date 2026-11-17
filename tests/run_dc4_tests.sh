#!/bin/sh
set -eu
sh tests/run_dc4_fx_tests.sh
sh tests/run_dc4_audio_tests.sh
for test in dc4_coef_cache dc4_taps; do
 cc -O1 -g -fsanitize=address -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen "tests/${test}_test.c" -lm -o "build/dc4/$test"
 ASAN_OPTIONS=detect_leaks=0 "build/dc4/$test"
done
for test in dual_core_ui_window dc3_voice_state; do
 cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -DOBV_FX_EXTERNAL=1 -Ifirmware/src -Ifirmware/gen "tests/${test}_test.c" build/dc4/fx.o -lm -o "build/dc4/$test"
 "build/dc4/$test"
done
node tests/dc4_installer_test.mjs
python3 tests/dc4_packages_test.py
node tests/backup_compatibility_test.mjs
node tests/dc4_controller_test.mjs
