#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Reproducible host-rendered auditions; no samples or model-generated audio."""
import json, subprocess, wave
from pathlib import Path
import numpy as np

root=Path(__file__).resolve().parents[1]
out=root/'build'/'voice-auditions';out.mkdir(parents=True,exist_ok=True)
exe=root/'build'/'host'/'voice_engine_test'
subprocess.run(['cc','-O1','-DOBV_HOST','-DBOARD_SMK37=1','-Ifirmware/src','-Ifirmware/gen',
                '-o',str(exe),'tests/voice_engine_test.c'],cwd=root,check=True)
names=['Choir_Aah','Ooh_Pad','Vox_Lead','Talkbox','Whisper','Chant','Vowel_Sweep','Dark_Choir',
       'ANALOG_e_VOICE','Choir_Aah_com_efeitos']
report=[]
for n,name in enumerate(names):
    raw=out/(name+'.raw')
    subprocess.run([str(exe),str(raw),str(n)],cwd=root,check=True,stdout=subprocess.DEVNULL)
    x=np.fromfile(raw,dtype='<i4').reshape(-1,2)
    assert np.count_nonzero(x)>1000
    assert np.max(np.abs(x.astype(np.int64)))<=32767
    report.append({'name':name,'frames':len(x),'peak':int(np.max(np.abs(x))),
                   'rms':float(np.sqrt(np.mean(x.astype(float)**2))),
                   'normalised':False,'end_fade_ms':80})
    # A file-boundary fade only: the native renderer is unchanged.
    xf=x.astype(float);fade=min(3528,len(xf));xf[-fade:]*=np.linspace(1,0,fade)[:,None]
    p=out/(f'{n+1:02d}_{name}.wav')
    with wave.open(str(p),'wb') as f:
        f.setnchannels(2);f.setsampwidth(2);f.setframerate(44100);f.writeframes(np.rint(xf).astype('<i2').tobytes())
    raw.unlink()
(out/'audio-measurements.json').write_text(json.dumps(report,indent=2)+'\n')
print(out)
