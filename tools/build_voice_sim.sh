#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Local experimental simulator only. No deployment or installable packaging.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build web/voice-lab
"${ZIG:-zig}" cc --target=wasm32-freestanding -mcpu=generic+bulk_memory -O2 \
    -fno-builtin -ffreestanding -nostdlib -Wno-unused-function -Wno-unused-variable \
    -Wno-unused-but-set-variable -Ifirmware/src -Ifirmware/gen \
    -Wl,--no-entry -Wl,--export-memory -Wl,--export-dynamic -Wl,-z,stack-size=262144 \
    -o build/drums4-raw.wasm web/sim/sim.c
"${WASM_OPT:-wasm-opt}" build/drums4-raw.wasm --enable-bulk-memory --asyncify \
    -O2 --strip-debug --strip-producers -o web/voice-lab/obervave-voice.wasm
echo 'Built experimental ANALOG + DRUM-X + SEQ TRANSPOSE simulator; not published and not a hardware firmware.'
