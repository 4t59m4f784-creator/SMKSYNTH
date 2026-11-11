#!/usr/bin/env python3
"""Inspect actual Elite images: pinning, model strings, loader names and seeded bank."""
import hashlib,json,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import fm1_install as I
import fm1pkg_make as M
import lz4blk
base=ROOT/'web/install-elite';pins=json.loads((base/'manifest.json').read_text())
apps=[]
for kind in ['release','update']:
 raw=(base/pins[kind]['url']).read_bytes();assert len(raw)==pins[kind]['size'] and hashlib.sha256(raw).hexdigest()==pins[kind]['sha']
 assert I.product_of(raw)=='SMK-37 Elite_19629'
 image,n=I.deinterleave(raw);assert n==20 and not I.package_damage(image)
 head=I._dec(image[:64]);files={}
 for k in range(struct.unpack_from('<H',head,8)[0]):
  e=I._dec(image[64+k*80:144+k*80]);off,size=struct.unpack_from('<II',e,8);files[struct.unpack_from('<H',e)[0]]=image[off:off+size]
 flash=files[0];area=bytearray(flash[0x4000:0x55000]);M.sfc(area,0,len(area),0,M.KEY)
 for identity in [b'SMK-37 Elite_19629',b'SMK-37 Elite_000',b'ota-SMK-37 Elite_016',b'1.0.3-elite2']:assert identity in area,identity
 apps.append(area)
 ota=files[100];pos=64;loader=b''
 while pos<len(ota):
  size,want=struct.unpack_from('<II',ota,pos);pos+=8;block=lz4blk.decompress(ota[pos:pos+size]);assert len(block)==want;loader+=block;pos+=size
 assert b'ota-SMK-37 Elite_900' in loader and b'ota-SMK-37 Pro_900' not in loader
 assert flash[0x55000:0x57000]==b'\xff'*8192
 expected=(ROOT/'factory/bank-SMK16P1.bin').read_bytes() if kind=='release' else b'\xff'*0x46000
 assert flash[0x57000:0x9D000]==expected
assert apps[0]==apps[1]
print('PASS: exact Elite pins, 20-block packages, Elite app/recovery/stock-return/custom-loader identities, identical apps, factory seed only in first install')
