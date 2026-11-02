# SPDX-License-Identifier: GPL-3.0-only
from pathlib import Path
import base64,hashlib,importlib.util,json,struct,tempfile,zlib
root=Path(__file__).resolve().parents[1];spec=importlib.util.spec_from_file_location('extractor',root/'tools/extract_author_bank.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
bank=bytearray(b'\xff'*0x46000);bank[:16]=b'SMK37DATASMK16P1'
def read(n):return (root/'build'/('fixture-'+n+'.bin')).read_bytes()
def sector(at,data):bank[at:at+12+len(data)]=struct.pack('<III',0x59474e44,len(data),zlib.crc32(data))+data
sector(0x41000,read('globals'));sector(0x42000,read('psect0'));sector(0x43000,read('psect1'));sector(0x44000,read('favorites'))
p,s=read('pattern'),read('scene');bank[4096:4096+16+len(p+s)]=struct.pack('<IIII',0x544f4c53,len(p),len(s),zlib.crc32(p+s))+p+s
(root/'build/b2-fixture-bank.bin').write_bytes(bank)
j=dict(format='smksynth-authoring-backup',version=1,layout='SMK16P1',model='SMK-37 Pro',firmware='19614',sha256=hashlib.sha256(bank).hexdigest(),bank=base64.b64encode(bank).decode())
with tempfile.TemporaryDirectory() as tmp:
 d=Path(tmp);f=d/'bank.smksynth';f.write_text(json.dumps(j));r=m.extract(f,d/'out');assert (d/'out/presets/71.patch80').read_bytes()[63:73]==b'BASS WORK ';assert (d/'out/presets/71.patch80').read_bytes()[73]==0xd0;assert (d/'out/patterns/01.seq4').read_bytes()==p;assert (d/'out/patterns/01.scene380').read_bytes()==s;assert (d/'out/favorites/bank.bin').read_bytes()==read('favorites');assert (d/'out/patterns/01-A.patch80').read_bytes()==(d/'out/presets/71.patch80').read_bytes()
 j['sha256']='0'*64;f.write_text(json.dumps(j))
 try:m.extract(f,d/'bad');raise RuntimeError('bad hash accepted')
 except AssertionError:assert not (d/'bad').exists()
print('PASS: actual native saved types -> authoring file -> exact preset/octave/favorite/pattern/scene extraction; SHA mismatch fails before output.')
