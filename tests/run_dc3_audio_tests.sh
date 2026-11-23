#!/bin/sh
set -eu
mkdir -p build/dc3
for mode in 0 1; do
 cc -O1 -g -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=$mode -Ifirmware/src -Ifirmware/gen tests/dual_core_audio_test.c -pthread -lm -o build/dc3/equiv-$mode
done
for plain in 0 1; do
 build/dc3/equiv-0 0 "$plain" > build/dc3/serial-$plain.pcm
 for failure in 0 1 2 3; do
  build/dc3/equiv-1 "$failure" "$plain" > build/dc3/worker-$plain-$failure.pcm
  cmp build/dc3/serial-$plain.pcm build/dc3/worker-$plain-$failure.pcm
 done
done
sha256sum build/dc3/serial-*.pcm build/dc3/worker-*.pcm
for sanitizer in thread address; do
 cc -O1 -g -fsanitize=$sanitizer -fno-omit-frame-pointer -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/dual_core_audio_test.c -pthread -lm -o build/dc3/equiv-$sanitizer
 for plain in 0 1; do
  ASAN_OPTIONS=detect_leaks=0 build/dc3/equiv-$sanitizer 2 "$plain" > /dev/null
 done
done
