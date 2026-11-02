#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Render the native sequencer, eight instruments per kit across a four-bar groove."""
import json,subprocess,wave
from pathlib import Path
import numpy as np
root=Path(__file__).resolve().parents[1];out=root/'build/drum-auditions';out.mkdir(parents=True,exist_ok=True)
exe=root/'build/host/drums_sequence_test'
subprocess.run(['cc','-O1','-DOBV_HOST','-DBOARD_SMK37=1','-Ifirmware/src','-Ifirmware/gen','-o',str(exe),'tests/drums_sequence_test.c'],cwd=root,check=True)
measure=[]
for kit,name in enumerate(['808_Boom','909_Club','606_Mini','707_Snap','DRUM-X']):
 raw=out/(name+'.raw');subprocess.run([str(exe),str(raw),str(kit)],cwd=root,check=True,stdout=subprocess.DEVNULL)
 x=np.fromfile(raw,dtype='<i4').reshape(-1,2);assert np.count_nonzero(x)>1000;assert np.max(np.abs(x.astype('int64')))<=32767
 measure.append({'kit':name,'frames':len(x),'peak':int(np.max(np.abs(x))),'rms':float(np.sqrt(np.mean(x.astype(float)**2))),'normalised':False,'end_fade_ms':80})
 xf=x.astype(float);xf[-3528:]*=np.linspace(1,0,3528)[:,None]
 with wave.open(str(out/(name+'.wav')),'wb') as w:
  w.setnchannels(2);w.setsampwidth(2);w.setframerate(44100);w.writeframes(np.rint(xf).astype('<i2').tobytes())
 raw.unlink()
(out/'measurements.json').write_text(json.dumps(measure,indent=2)+'\n')
print(out)
