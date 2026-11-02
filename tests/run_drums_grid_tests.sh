#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host validation
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/drums_grid_test tests/drums_grid_test.c
build/host/drums_grid_test build/drums4-grid.ppm > validation/drums4-grid.log
# VD2 fixes gain/retrigger artifacts, so PCM is intentionally different.
# Both legacy and current sequencer workflows must still execute successfully.
ref=drums3-preserved
if ! git rev-parse --verify "$ref" >/dev/null 2>&1; then ref=origin/drums3-preserved; fi
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
git archive "$ref" | tar -x -C "$tmp"
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/drum-groove-current tests/drums_sequence_test.c
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 "-I$tmp/firmware/src" "-I$tmp/firmware/gen" "-DOBV_TEST_SOURCE=\"$tmp/firmware/src/obervave.c\"" -o build/host/drum-groove-baseline tests/drums_sequence_test.c
build/host/drum-groove-current build/host/drum-groove-current.raw > validation/drums4-groove-current.log
build/host/drum-groove-baseline build/host/drum-groove-baseline.raw > validation/drums4-groove-baseline.log
# Do not enforce bug-compatible audio; timing/masks/transport are asserted by drums_grid_test and drums_sequence_test.
sha256sum build/host/drum-groove-current.raw build/host/drum-groove-baseline.raw > validation/drums4-groove-equivalence.log
node tests/drums_grid_wasm_test.mjs > validation/drums4-wasm.log 2>&1
printf '%s\n' 'PASS: eight-lane native grid, legacy/extended storage, legacy and current DRUMS workflows (gain-fixed PCM intentionally differs) and WASM pattern save/copy/reboot/record.'
