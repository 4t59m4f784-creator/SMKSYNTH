#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Instrument a disposable firmware copy, never production sources.
from pathlib import Path
import shutil
import sys
repo=Path(__file__).resolve().parents[1];source=Path(sys.argv[1]).resolve() if len(sys.argv)>1 else repo;dst=repo/'build/gain-audit'
shutil.copytree(source/'firmware',dst/'firmware',dirs_exist_ok=True);(dst/'tests').mkdir(exist_ok=True);shutil.copy2(source/'tests/host.c',dst/'tests/host.c')
p=dst/'firmware/src/obv_fx.h';s=p.read_text().replace('static inline int32_t sat16(int32_t v) { return v > 32767 ? 32767 : v < -32767 ? -32767 : v; }','static unsigned audit_stage;static uint64_t audit_clip[4];static int32_t audit_peak[4];\nstatic inline int32_t sat16(int32_t v) {int32_t a=v<0?-v:v;if(a>audit_peak[audit_stage])audit_peak[audit_stage]=a;if(a>32767)audit_clip[audit_stage]++;return v > 32767 ? 32767 : v < -32767 ? -32767 : v; }');p.write_text(s)
p=dst/'firmware/src/obv_engine.h';s=p.read_text().replace('    for (i = 0; i < n; i++) {                   /* the sends at', '    audit_stage=1;\n    for (i = 0; i < n; i++) {                   /* the sends at').replace('    fx_process(out, snd_c, snd_d, snd_r, n);','    audit_stage=2;fx_process(out, snd_c, snd_d, snd_r, n);').replace('    drive_process(out, snd_v, n);','    audit_stage=3;drive_process(out, snd_v, n);');p.write_text(s)

import subprocess
(repo/'build/host').mkdir(parents=True,exist_ok=True)
subprocess.run(['cc','-O2','-DOBV_HOST','-DBOARD_SMK37=1','-I'+str(dst/'firmware/src'),'-I'+str(dst/'firmware/gen'),'-DOBV_TEST_SOURCE="'+str(dst/'firmware/src/obervave.c')+'"','-o',str(repo/'build/host/voice-gain-probe'),str(repo/'tools/voice_gain_probe.c'),'-lm'],check=True)
print('Built build/host/voice-gain-probe. Arguments: VOICE(1)/ANALOG(0), preset, raw-volume, wet(0/1), note-count, base-MIDI-note.')
