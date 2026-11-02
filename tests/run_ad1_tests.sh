#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host validation
for test in analog_drums_transpose_test drums_engine_test drums_grid_test drums_workflow_test drums_sequence_test; do
 cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o "build/host/$test" "tests/$test.c"
 "build/host/$test" > "validation/ad1-$test.log" 2>&1
done
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/ad-analog tests/analog_equivalence.c
build/host/ad-analog > build/host/ad-analog.pcm 2> validation/ad1-analog.log
printf '%s\n' '4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932  build/host/ad-analog.pcm' | sha256sum -c - > validation/ad1-analog-equivalence.log
build/host/drums_sequence_test build/host/ad-drums.pcm >> validation/ad1-drums_sequence_test.log
printf '%s\n' '5ac7a82852462679e74d0976eefb6d5183bf4aba9619079ce2b50ea84f837dfa  build/host/ad-drums.pcm' | sha256sum -c - > validation/ad1-drums-equivalence.log
cc -O1 -g -fsanitize=undefined,address -fno-sanitize-recover=all -Ifirmware/src -Ifirmware/gen -o build/host/drum_dsp_sanitizer tests/drum_dsp_sanitizer.c
ASAN_OPTIONS=detect_leaks=0 build/host/drum_dsp_sanitizer > validation/ad1-drums-sanitizer.log 2>&1
# Build the WASM first (tools/build_voice_sim.sh). Installer binaries must match
# web/keyboard-ad1/manifest.json; restore their exact bytes from the release bundle.
for test in ad1_transpose_wasm_test drums_grid_wasm_test ad1_worklet_test ad1_installer_test ad1_controller_test; do
 node "tests/$test.mjs" > "validation/ad1-$test.log" 2>&1
done
printf '%s\n' 'PASS: AD1 native, golden PCM, DRUM-X sanitizer, WASM panel/storage, worklet, installer and controller.'
