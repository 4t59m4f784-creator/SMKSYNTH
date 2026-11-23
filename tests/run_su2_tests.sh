#!/bin/sh
set -eu
mkdir -p build/su1 build/su2 build/supersaw
cc -O1 -g -fsanitize=address,undefined -fno-sanitize=shift -fno-sanitize-recover=all -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/su2_workflow_test.c -lm -o build/su2/workflow
ASAN_OPTIONS=detect_leaks=0 build/su2/workflow
for name in sub_osc sub_stress sub_split_layer sub_super su2_sub_additive; do
 cc -O2 -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/${name}_test.c -lm -o build/su2/$name
 build/su2/$name
done
sh tests/run_sub_audio_tests.sh
node tests/su2_installer_test.mjs
python3 tests/su2_packages_test.py
cc -O2 -o build/backup_server tests/backup_server.c
node tests/su2_backup_test.mjs --protocol
node tests/su2_bank_test.mjs
cc -O2 -DBOARD_SMK37=1 -DOBV_VOICE_HW=1 -Ifirmware/hal tests/backup_loader_test.c -o build/su2/flash-check
build/su2/flash-check experimental/sub-osc-su1/SMKSYNTH-1.2.2-SU1-Pro.fwsc experimental/endless-midi-su2/SMKSYNTH-1.2.2-SU2-Pro.fwsc
cc -O2 -DBOARD_SMK37=1 -DOBV_VOICE_HW=1 '-DFELUCCA_ID="ota-SMK-37 Elite_900"' -Ifirmware/hal tests/backup_loader_test.c -o build/su2/flash-elite
build/su2/flash-elite experimental/sub-osc-su1/SMKSYNTH-1.2.2-SU1-Elite.fwsc experimental/endless-midi-su2/SMKSYNTH-1.2.2-SU2-Elite.fwsc
