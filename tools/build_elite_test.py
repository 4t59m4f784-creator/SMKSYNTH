#!/usr/bin/env python3
"""Rebuild Elite2 from the checked-out SMKSYNTH 1.0.3 sources; never flashes hardware."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--sdk',required=True);a=p.parse_args()
dest=ROOT/'web/install-elite';pins=json.loads((dest/'manifest.json').read_text())
for kind in ['release','update']:
    command=[sys.executable,'tools/build.py','--board','smk37','--model','elite','--release','1.0.3-elite2','--experimental-id','19629','--voice-hardware','--sdk',a.sdk]
    if kind=='release':command+=['--first-install','--factory-bank','factory/bank-SMK16P1.bin']
    subprocess.run(command,cwd=ROOT,check=True)
    built=ROOT/'build/obervave-1.0.3-elite2-smk37-elite.fwsc'
    data=built.read_bytes();pin=pins[kind]
    if len(data)!=pin['size'] or hashlib.sha256(data).hexdigest()!=pin['sha']:
        raise SystemExit('Rebuilt package differs from the reviewed Elite2 pin: '+kind)
    shutil.copy2(built,dest/pin['url'])
print('PASS: both Elite2 packages reproduced exactly; no hardware accessed')
