#!/bin/sh
set -eu
mkdir -p build/dc4
cc -O2 -fwrapv -Ifirmware/src -Ifirmware/gen -c firmware/src/obv_fx.c -o build/dc4/fx.o
for mode in 0 1; do
 cc -O1 -g -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=$mode -DOBV_FX_EXTERNAL=1 -Ifirmware/src -Ifirmware/gen tests/dual_core_audio_test.c build/dc4/fx.o -pthread -lm -o build/dc4/equiv-$mode
done
for plain in 0 1; do
 build/dc4/equiv-0 0 "$plain" > build/dc4/serial-$plain.pcm
 for failure in 0 1 2 3; do
  build/dc4/equiv-1 "$failure" "$plain" > build/dc4/worker-$plain-$failure.pcm
  cmp build/dc4/serial-$plain.pcm build/dc4/worker-$plain-$failure.pcm
 done
done
sha256sum build/dc4/serial-*.pcm build/dc4/worker-*.pcm
for sanitizer in thread address; do
 cc -O2 -g -fwrapv -fsanitize=$sanitizer -Ifirmware/src -Ifirmware/gen -c firmware/src/obv_fx.c -o build/dc4/fx-$sanitizer.o
 cc -O1 -g -fsanitize=$sanitizer -fno-omit-frame-pointer -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -DOBV_FX_EXTERNAL=1 -Ifirmware/src -Ifirmware/gen tests/dual_core_audio_test.c build/dc4/fx-$sanitizer.o -pthread -lm -o build/dc4/equiv-$sanitizer
 for plain in 0 1; do ASAN_OPTIONS=detect_leaks=0 build/dc4/equiv-$sanitizer 2 "$plain" > /dev/null; done
done
