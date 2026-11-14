#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/controls build/mo1
cc -O1 -g -fsanitize=address,undefined -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/controls/physical-sanitized tests/physical_controls_test.c
ASAN_OPTIONS=detect_leaks=0 build/controls/physical-sanitized
for name in midi_layer endless endless_midi super_limit midi_abs_acceleration midi_overview zoom_palette preset_name midi_backpressure midi_mode midi_mode_transition live_parameter_release midi_controller_audit midi_parts sustain_panic seq_dp2 octave split_layer_rows save_focus save_display patterns16 drum_pads smk_scene_recall smk_boot_header; do
 cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o "build/controls/$name" "tests/${name}_test.c"
 "build/controls/$name"
done
