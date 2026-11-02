#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
sh tests/run_latency_tests.sh
cc -O1 -Wall -Wno-unused-function -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/perf_test tests/smk_performance_test.c
build/host/perf_test
cc -O1 -DBOARD_SMK37=1 -Ifirmware/hal -o build/host/smk_flash_test tests/smk_flash_test.c
build/host/smk_flash_test
