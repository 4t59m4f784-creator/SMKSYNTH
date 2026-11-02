#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -O1 -Wall -Wno-unused-function -Wno-int-to-pointer-cast -Ifirmware/hal -o build/host/smk_latency_contacts_test tests/smk_latency_contacts_test.c
build/host/smk_latency_contacts_test
cc -O1 -Wall -Wno-unused-function -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/smk_latency_dispatch_test tests/smk_latency_dispatch_test.c
build/host/smk_latency_dispatch_test
python3 tests/smk_ui_test.py
