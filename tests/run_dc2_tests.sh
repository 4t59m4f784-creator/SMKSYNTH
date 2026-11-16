#!/bin/sh
set -eu
sh tests/run_dual_core_tests.sh
cc -O2 tests/audio_profile_window_test.c -o build/dc1/profile-window
build/dc1/profile-window
cc -O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/dual_core_copy_test.c -lm -o build/dc1/copy-test
ASAN_OPTIONS=detect_leaks=0 build/dc1/copy-test
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/dual_core_ui_window_test.c -lm -o build/dc1/ui-window
build/dc1/ui-window
node tests/dc2_installer_test.mjs
python3 tests/dc2_packages_test.py
