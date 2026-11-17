#!/bin/sh
set -eu
mkdir -p build/dc4
# The inline reference retains the original equations; the separate object is -O2.
cc -O2 -fwrapv -Ifirmware/src -Ifirmware/gen -c firmware/src/obv_fx.c -o build/dc4/fx.o
for opt in 0 1; do
 cc -O1 -g -DOBV_HOST -DBOARD_SMK37=1 -DOBV_FX_EXTERNAL=$opt -Ifirmware/src -Ifirmware/gen tests/dc4_fx_test.c $(test "$opt" = 0 || echo build/dc4/fx.o) -lm -o build/dc4/fx-$opt
 build/dc4/fx-$opt > build/dc4/fx-$opt.pcm
done
cmp build/dc4/fx-0.pcm build/dc4/fx-1.pcm
sha256sum build/dc4/fx-*.pcm
# ASan checks the separately compiled object and its caller together.
cc -O2 -g -fwrapv -fsanitize=address -Ifirmware/src -Ifirmware/gen -c firmware/src/obv_fx.c -o build/dc4/fx-asan.o
cc -O1 -g -fsanitize=address -DOBV_HOST -DBOARD_SMK37=1 -DOBV_FX_EXTERNAL=1 -Ifirmware/src -Ifirmware/gen tests/dc4_fx_test.c build/dc4/fx-asan.o -lm -o build/dc4/fx-asan
ASAN_OPTIONS=detect_leaks=0 build/dc4/fx-asan > /dev/null
