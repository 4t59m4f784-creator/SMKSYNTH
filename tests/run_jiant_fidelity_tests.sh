#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host validation
python3 tools/gen_jiant_voice.py
for name in jiant_fidelity_test jiant_master_test jiant_control_test jiant_profile_switch_test; do
 cc -O1 -fwrapv -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o "build/host/$name" "tests/$name.c"
 "build/host/$name" > "validation/$name.log"
done
cc -O1 -g -fsanitize=undefined,address -fno-sanitize-recover=all -fno-omit-frame-pointer -Ifirmware/src -Ifirmware/gen -o build/host/jv-sanitizer tests/formant_dsp_sanitizer.c
ASAN_OPTIONS=detect_leaks=0 build/host/jv-sanitizer > validation/jv-sanitizer.log
printf '%s\n' 'PASS: pinned JIANT kernel, default master, envelope/velocity and isolated DSP. Native tests only.'
