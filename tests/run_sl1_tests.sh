#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host validation
for test in split_layer_rows_test split_layer_test split_layer_stress_test analog_drums_transpose_test drums_engine_test drums_grid_test drums_workflow_test drums_sequence_test; do
 cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o "build/host/$test" "tests/$test.c"
 "build/host/$test" > "validation/sl1-$test.log" 2>&1
done
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/single-analog tests/analog_equivalence.c
build/host/single-analog > build/host/single-analog.pcm 2> validation/sl1-analog.log
printf '%s\n' '4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932  build/host/single-analog.pcm' | sha256sum -c - > validation/sl1-analog-equivalence.log
build/host/drums_sequence_test build/host/sl1-drums.pcm >> validation/sl1-drums_sequence_test.log
printf '%s\n' '5ac7a82852462679e74d0976eefb6d5183bf4aba9619079ce2b50ea84f837dfa  build/host/sl1-drums.pcm' | sha256sum -c - > validation/sl1-drums-equivalence.log
cc -O1 -g -fsanitize=undefined,address -fno-sanitize-recover=all -Ifirmware/src -Ifirmware/gen -o build/host/drum_dsp_sanitizer tests/drum_dsp_sanitizer.c
ASAN_OPTIONS=detect_leaks=0 build/host/drum_dsp_sanitizer > validation/sl1-drums-sanitizer.log 2>&1
printf '%s\n' 'PASS: SL1 native routing/REC/storage/stress, AD1 regressions, exact PCM, DRUM-X sanitizer.'
