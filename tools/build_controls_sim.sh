#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Build the release simulator with physical fader behavior.
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/controls
"${ZIG:-zig}" cc --target=wasm32-freestanding -mcpu=generic+bulk_memory -O2 \
 '-DAPP_VER="1.1.0"' -DOBV_EXPERIMENTAL=1 -fno-builtin -ffreestanding -nostdlib -Wno-unused-function -Wno-unused-variable \
 -Wno-unused-but-set-variable -Ifirmware/src -Ifirmware/gen \
 -Wl,--no-entry -Wl,--export-memory -Wl,--export-dynamic -Wl,-z,stack-size=262144 \
 -o build/controls/controls-raw.wasm web/sim/sim.c
"${WASM_OPT:-wasm-opt}" build/controls/controls-raw.wasm --enable-bulk-memory --asyncify \
 -O2 --strip-debug --strip-producers -o build/controls/SMKSYNTH-1.1.0.wasm
