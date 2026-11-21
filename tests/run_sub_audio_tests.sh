#!/bin/sh
set -eu
mkdir -p build/su1
for mode in 0 1; do
 cc -O1 -g -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=$mode -Ifirmware/src -Ifirmware/gen tests/sub_dual_audio_test.c -pthread -lm -o build/su1/audio-$mode
done
for plain in 0 1; do
 build/su1/audio-0 0 "$plain" > build/su1/serial-$plain.pcm
 for failure in 0 1 2 3; do
  build/su1/audio-1 "$failure" "$plain" > build/su1/worker-$plain-$failure.pcm
  cmp build/su1/serial-$plain.pcm build/su1/worker-$plain-$failure.pcm
 done
done
for sanitizer in address thread; do
 cc -O1 -g -fsanitize=$sanitizer -fno-omit-frame-pointer -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/sub_dual_audio_test.c -pthread -lm -o build/su1/parallel-$sanitizer
 ASAN_OPTIONS=detect_leaks=0 build/su1/parallel-$sanitizer 2 0 > /dev/null
done
