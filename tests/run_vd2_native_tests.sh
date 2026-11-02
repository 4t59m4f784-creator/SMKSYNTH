#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host validation
for name in smk_pf12_test smk_pf8_test smk_wheel_track_test smk_slot_led_test smk_scene_recall_test smk_boot_header_test smk_encoder_matrix_test multiengine_test voice_engine_test voice_workflow_test drums_engine_test drums_workflow_test drums_sequence_test drums_grid_test vd2_controller_audio_test; do
 cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o "build/host/$name" "tests/$name.c"
 "build/host/$name" > "validation/vd2-$name.log" 2>&1
done
cc -O1 -DEXPECT_IDLE_WHEELS -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/vd2-idle tests/smk_wheel_idle_test.c
build/host/vd2-idle > validation/vd2-wheel-idle.log
for name in formant_dsp_sanitizer drum_dsp_sanitizer; do
 cc -O1 -g -fsanitize=undefined,address -fno-sanitize-recover=all -fno-omit-frame-pointer -Ifirmware/src -Ifirmware/gen -o "build/host/$name" "tests/$name.c"
 ASAN_OPTIONS=detect_leaks=0 "build/host/$name" > "validation/vd2-$name.log" 2>&1
done
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/analog-current tests/analog_equivalence.c
build/host/analog-current > build/host/analog-current.pcm
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -DOBV_VD1_WHEEL_EMULATION -Ifirmware/src -Ifirmware/gen -o build/host/voice-current tests/voice_equivalence.c
build/host/voice-current > build/host/voice-current.pcm
printf '%s\n' 'PASS: VD2 native controls, storage, engines, drum grid, wheel retention, retrigger and isolated DSP sanitizers. No hardware CPU or physical validation implied.'
