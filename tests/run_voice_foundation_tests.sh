#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host validation
sh tests/run_performance_tests.sh > validation/performance.log 2>&1
for voice_test in smk_pf12_test smk_pf8_test smk_wheel_track_test smk_slot_led_test smk_scene_recall_test smk_boot_header_test smk_encoder_matrix_test multiengine_test voice_engine_test voice_workflow_test; do
    cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o "build/host/$voice_test" "tests/$voice_test.c"
    "build/host/$voice_test" > "validation/$voice_test.log" 2>&1
done
cc -O1 -DEXPECT_IDLE_WHEELS -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/smk_wheel_idle_test tests/smk_wheel_idle_test.c
build/host/smk_wheel_idle_test > validation/smk_wheel_idle_test.log

# The preserved branch contains the exact imported PF12 snapshot.
voice_baseline_dir=$(mktemp -d)
trap 'rm -rf "$voice_baseline_dir"' EXIT HUP INT TERM
voice_baseline_ref=pf12-preserved
if ! git rev-parse --verify "$voice_baseline_ref" >/dev/null 2>&1; then
    voice_baseline_ref=origin/pf12-preserved
fi
git archive "$voice_baseline_ref" | tar -x -C "$voice_baseline_dir"
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/analog-current tests/analog_equivalence.c
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 "-I$voice_baseline_dir/firmware/src" "-I$voice_baseline_dir/firmware/gen" \
    "-DOBV_TEST_SOURCE=\"$voice_baseline_dir/firmware/src/obervave.c\"" -o build/host/analog-baseline tests/analog_equivalence.c
build/host/analog-current > build/host/analog-current.pcm
build/host/analog-baseline > build/host/analog-baseline.pcm
cmp build/host/analog-current.pcm build/host/analog-baseline.pcm
sha256sum build/host/analog-current.pcm build/host/analog-baseline.pcm > validation/analog-equivalence.log

cc -O1 -DOBV_HOST -DBOARD_SMK37=1 "-I$voice_baseline_dir/firmware/src" "-I$voice_baseline_dir/firmware/gen" \
    -o build/host/matrix-baseline "$voice_baseline_dir/tests/smk_encoder_matrix_test.c"
build/host/matrix-baseline > validation/matrix-baseline.log
python3 - <<'MATRIX'
from pathlib import Path
baseline=Path('validation/matrix-baseline.log').read_text().splitlines()
current=[line for line in Path('validation/smk_encoder_matrix_test.log').read_text().splitlines() if line.startswith('MATRIX ') and int(line.split()[1])<21]
assert current==baseline, 'legacy encoder matrix changed'
MATRIX
printf '%s\n' 'PASS: encoder matrix identical to preserved PF12' > validation/matrix-equivalence.log

# Package generation must fail before writing a hardware build.
if python3 tools/build.py > validation/installable-build-gate.log 2>&1; then
    echo 'FAIL: installable build unexpectedly enabled' >&2
    exit 1
fi
node tests/voice_wasm_test.mjs > validation/voice2-wasm-runtime.log 2>&1
cc -O1 -g -fsanitize=undefined,address -fno-sanitize-recover=all -fno-omit-frame-pointer -Ifirmware/src -Ifirmware/gen -o build/host/formant-dsp-sanitizer tests/formant_dsp_sanitizer.c
ASAN_OPTIONS=detect_leaks=0 build/host/formant-dsp-sanitizer > validation/voice2-dsp-sanitizer.log 2>&1
echo 'PASS: VOICE prototype, audible PF12 regression, native workflows, WASM/worklet and isolated DSP checks; no hardware validation implied'
