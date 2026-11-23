#!/bin/sh
set -eu
mkdir -p build/dc1
for mode in 0 1; do
 cc -O1 -g -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=$mode -Ifirmware/src -Ifirmware/gen tests/dual_core_audio_test.c -pthread -lm -o build/dc1/equiv-$mode
done
build/dc1/equiv-0 > build/dc1/serial.pcm
for failure in 0 1 2 3; do
 build/dc1/equiv-1 "$failure" > build/dc1/worker-$failure.pcm
 cmp build/dc1/serial.pcm build/dc1/worker-$failure.pcm
done
sha256sum build/dc1/serial.pcm build/dc1/worker-*.pcm
cc -O1 -g -fsanitize=thread -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/dual_core_audio_test.c -pthread -lm -o build/dc1/equiv-tsan
build/dc1/equiv-tsan > /dev/null
cc -O1 -g -fsanitize=address -fno-omit-frame-pointer -DOBV_HOST -DBOARD_SMK37=1 -DOBV_DUAL_CORE=1 -Ifirmware/src -Ifirmware/gen tests/dual_core_audio_test.c -pthread -lm -o build/dc1/equiv-asan
ASAN_OPTIONS=detect_leaks=0 build/dc1/equiv-asan 2 > /dev/null
