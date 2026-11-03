#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
from pathlib import Path
import hashlib,json,struct
import fm1_install as I,fm1pkg_make as M
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'experimental/controls-ct1'
def flash(p):
 image,_=I.deinterleave(p.read_bytes());e=I._dec(image[0x40:0x90]);o,n=struct.unpack_from('<II',e,8);return image[o:o+n]
def main():
 manifest=json.loads((OUT/'manifest.json').read_text())
 for name,item in manifest['files'].items():
  p=OUT/name;assert p.stat().st_size==item['bytes'];assert hashlib.sha256(p.read_bytes()).hexdigest()==item['sha256'],name
 for name,sha in manifest['source_sha256'].items():assert hashlib.sha256((ROOT/name).read_bytes()).hexdigest()==sha,name+' changed; rebuild the candidate'
 original=flash(ROOT/'web/install/firmware/SMKSYNTH-1.0.0-SMK37-Pro.fwsc');returned=flash(OUT/'SMKSYNTH-1.0.0-return-preserve-bank.fwsc')
 assert original[0x4000:0x55000]==returned[0x4000:0x55000], 'return app differs from the public release'
 for name in ['SMKSYNTH-1.0.1-ct1-SMK37-Pro.fwsc','SMKSYNTH-1.0.0-return-preserve-bank.fwsc']:
  f=flash(OUT/name);assert f[0x55000:0x9d000]==b'\xff'*0x48000
 print('PASS: candidate/source SHA-256 pins, preserved record area, exact public 1.0.0 app in return package')
if __name__=='__main__':main()
