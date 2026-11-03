#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Package CT1 plus a bank-preserving return to the exact public 1.0.0 app.
Run build.py in the documented experimental profile and build_controls_sim.sh first.
Does not touch the public installer, firmware, simulator or frozen release ZIP.
"""
from pathlib import Path
import hashlib,json,struct,shutil
import fm1_install as I,fm1pkg_make as M
ROOT=Path(__file__).resolve().parents[1]
def main():
 out=ROOT/'experimental/controls-ct1';out.mkdir(parents=True,exist_ok=True)
 original=ROOT/'web/install/firmware/SMKSYNTH-1.0.0-SMK37-Pro.fwsc'
 assert hashlib.sha256(original.read_bytes()).hexdigest()=='47b625307c97dea73efbb5148076b091f1c1568ca1d1e3a2ae98d146b20d4c0d'
 image,_=I.deinterleave(original.read_bytes());entry=I._dec(image[0x40:0x90]);off,size=struct.unpack_from('<II',entry,8)
 flash=bytearray(image[off:off+size]);flash[0x55000:0x9d000]=b'\xff'*0x48000
 ota=(ROOT/'build/loader/ota.bin').read_bytes()
 # The update loader was compiled with OBV_VOICE_HW, without FIRST_INSTALL:
 # a recognized SMK16P1 bank is required and every byte in it is preserved.
 M.set_board('smk37');M.APP_SLOT=0x50000
 restored=M.ufw(bytes(flash),ota,'SMK-37 Pro_19618')
 (out/'SMKSYNTH-1.0.0-return-preserve-bank.fwsc').write_bytes(restored)
 candidate=ROOT/'build/obervave-1.0.1-ct1-smk37.fwsc'
 shutil.copy2(candidate,out/'SMKSYNTH-1.0.1-ct1-SMK37-Pro.fwsc')
 shutil.copy2(ROOT/'build/controls/SMKSYNTH-1.0.1-ct1.wasm',out/'SMKSYNTH-1.0.1-ct1.wasm')
 source_paths=sorted([*ROOT.glob('firmware/**/*.[ch]'),ROOT/'web/sim/sim.c',ROOT/'tools/build.py',ROOT/'tools/build_controls_sim.sh'])
 source_sha256={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest()for p in source_paths}
 files=[out/n for n in ['SMKSYNTH-1.0.1-ct1-SMK37-Pro.fwsc','SMKSYNTH-1.0.0-return-preserve-bank.fwsc','SMKSYNTH-1.0.1-ct1.wasm']]
 hashes={p.name:hashlib.sha256(p.read_bytes()).hexdigest()for p in files}
 (out/'SHA256SUMS.txt').write_text(''.join(f'{h}  {n}\n'for n,h in hashes.items()))
 (out/'manifest.json').write_text(json.dumps(dict(version='1.0.1-ct1',status='hardware-test-candidate',identity='SMK-37 Pro_19618',origin='installed SMKSYNTH 1.0.0 with valid SMK16P1 bank',first_install=False,bank_preserving=True,physical_validation=False,source_sha256=source_sha256,files={p.name:dict(bytes=p.stat().st_size,sha256=hashes[p.name])for p in files}),indent=2)+'\n')
 print(json.dumps(hashes))
if __name__=='__main__':main()
