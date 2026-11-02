#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host validation
for name in drums_engine_test drums_workflow_test drums_sequence_test; do
 cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o "build/host/$name" "tests/$name.c"
 "build/host/$name" > "validation/$name.log" 2>&1
done
ref=voice2-preserved
if ! git rev-parse --verify "$ref" >/dev/null 2>&1; then ref=origin/voice2-preserved; fi
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
git archive "$ref" | tar -x -C "$tmp"
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -DOBV_VD1_WHEEL_EMULATION -o build/host/voice-current tests/voice_equivalence.c
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 "-I$tmp/firmware/src" "-I$tmp/firmware/gen" "-DOBV_TEST_SOURCE=\"$tmp/firmware/src/obervave.c\"" -o build/host/voice-baseline tests/voice_equivalence.c
build/host/voice-current > build/host/voice-current.pcm
build/host/voice-baseline > build/host/voice-baseline.pcm
cmp build/host/voice-current.pcm build/host/voice-baseline.pcm
sha256sum build/host/voice-current.pcm build/host/voice-baseline.pcm > validation/voice-equivalence.log
cc -O1 -g -fsanitize=undefined,address -fno-sanitize-recover=all -fno-omit-frame-pointer -Ifirmware/src -Ifirmware/gen -o build/host/drum-dsp-sanitizer tests/drum_dsp_sanitizer.c
ASAN_OPTIONS=detect_leaks=0 build/host/drum-dsp-sanitizer > validation/drums-dsp-sanitizer.log 2>&1
node tests/drums_wasm_test.mjs > validation/drums3-wasm-runtime.log 2>&1
printf '%s\n' 'PASS: DRUMS native DSP, UI and storage, preserved VOICE DSP under baseline controller emulation, WASM/worklet. Hardware not tested.'
