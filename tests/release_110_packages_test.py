#!/usr/bin/env python3
import hashlib,json,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import fm1_install as I
import fm1pkg_make as M
import lz4blk
base=ROOT/'web/install';allpins=json.loads((base/'manifest.json').read_text())
for model in ['pro','elite']:
 pins=allpins[model];name=pins['model'].encode();apps=[]
 for kind in ['release','update']:
  raw=(base/pins[kind]['url']).read_bytes();assert len(raw)==pins[kind]['size'] and hashlib.sha256(raw).hexdigest()==pins[kind]['sha']
  assert I.product_of(raw)==pins[kind]['product']
  image,n=I.deinterleave(raw);assert n==20 and not I.package_damage(image)
  head=I._dec(image[:64]);files={}
  for k in range(struct.unpack_from('<H',head,8)[0]):
   e=I._dec(image[64+k*80:144+k*80]);off,size=struct.unpack_from('<II',e,8);files[struct.unpack_from('<H',e)[0]]=image[off:off+size]
  flash=files[0];area=bytearray(flash[0x4000:0x55000]);M.sfc(area,0,len(area),0,M.KEY)
  for identity in [name+b'_19649',name+b'_000',b'ota-'+name+b'_016']:assert identity in area,identity
  apps.append(area)
  ota=files[100];pos=64;loader=b''
  while pos<len(ota):
   size,want=struct.unpack_from('<II',ota,pos);pos+=8;block=lz4blk.decompress(ota[pos:pos+size]);assert len(block)==want;loader+=block;pos+=size
  assert b'ota-'+name+b'_900' in loader and (b'ota-SMK-37 Pro_900' if model=='elite' else b'ota-SMK-37 Elite_900') not in loader
  assert flash[0x55000:0x57000]==b'\xff'*8192
  expected=(ROOT/'factory/bank-SMK16P1.bin').read_bytes() if kind=='release' else b'\xff'*0x46000
  assert flash[0x57000:0x9D000]==expected
 assert apps[0]==apps[1]
print('PASS: all four final Pro/Elite pins, 20-block packages, Elite app/recovery/stock-return/custom-loader identities, identical apps, factory seed only in first install')
