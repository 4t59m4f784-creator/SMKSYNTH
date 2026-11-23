#!/bin/sh
set -eu
sh tests/run_dc3_audio_tests.sh
cc -O2 tests/audio_profile_window_test.c -o build/dc3/profile-window
build/dc3/profile-window
cc -O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/dual_core_copy_test.c -lm -o build/dc3/copy-test
ASAN_OPTIONS=detect_leaks=0 build/dc3/copy-test
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/dual_core_ui_window_test.c -lm -o build/dc3/ui-window
build/dc3/ui-window
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/dc3_voice_state_test.c -lm -o build/dc3/voice-state
build/dc3/voice-state
node tests/dc3_installer_test.mjs
python3 tests/dc3_packages_test.py
node tests/backup_compatibility_test.mjs
