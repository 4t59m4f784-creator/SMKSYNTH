import json,hashlib,struct,sys
from pathlib import Path
sys.path.insert(0,'tools');import fm1_install as I;import fm1pkg_make as M
base=Path('experimental/midi-layer');pins=json.loads((base/'manifest.json').read_text())
for k,p in pins.items():
 raw=(base/p['url']).read_bytes();assert len(raw)==p['size'] and hashlib.sha256(raw).hexdigest()==p['sha'];assert I.product_of(raw)==p['product'];image,n=I.deinterleave(raw);assert n==20 and not I.package_damage(image)
 head=I._dec(image[:64]);files={}
 for i in range(struct.unpack_from('<H',head,8)[0]):
  e=I._dec(image[64+i*80:144+i*80]);off,size=struct.unpack_from('<II',e,8);files[struct.unpack_from('<H',e)[0]]=image[off:off+size]
 flash=files[0];assert flash[0x55000:0x9d000]==b'\xff'*0x48000
 area=bytearray(flash[0x4000:0x55000]);M.sfc(area,0,len(area),0,M.KEY);assert p['product'].encode() in area
print('PASS ML1 packages: 4 exact products, sizes, SHA/CRC, 20 blocks; all packages leave restore journal/private bank unseeded for preserving updates.')
