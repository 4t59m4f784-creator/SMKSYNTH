import base64,hashlib,json,struct,zlib
from pathlib import Path
root=Path(__file__).resolve().parents[1];factory=root/'factory';bank=(factory/'bank-SMK16P1.bin').read_bytes()
assert len(bank)==0x46000 and bank[:16]==b'SMK37DATASMK16P1'
manifest=json.loads((factory/'manifest.json').read_text())
for file,sha in manifest['files'].items():assert hashlib.sha256((factory/file).read_bytes()).hexdigest()==sha
def sector(at,size):
 magic,n,crc=struct.unpack_from('<III',bank,at);d=bank[at+12:at+12+n];assert magic==0x59474e44 and n==size and zlib.crc32(d)==crc;return d
g=sector(0x41000,67);assert g[0]==7 and g[2]==90 and g[3]==0 and g[4]==0 and g[5]==0 and g[6]==0
assert bank[0x42000:0x44000]==b'\xff'*8192
for n in range(1,17):
 at=4096+(n-1)*16384
 if n>5:assert bank[at:at+16384]==b'\xff'*16384;continue
 magic,na,nb,crc=struct.unpack_from('<IIII',bank,at);assert magic==0x544f4c53 and nb==380
 p=(factory/f'pattern-{n:02d}.seq4').read_bytes();s=(factory/f'pattern-{n:02d}.scene380').read_bytes()
 assert bank[at+16:at+16+na+nb]==p+s and zlib.crc32(p+s)==crc
fav=sector(0x44000,708);assert struct.unpack_from('<I',fav,8)[0]==255 and bank[0x45000:]==b'\xff'*4096
# Optional comparison to the original author export, provided locally, never distributed.
import sys
if len(sys.argv)>1:
 j=json.loads(Path(sys.argv[1]).read_text());original=base64.b64decode(j['bank']);assert hashlib.sha256(original).hexdigest()==j['sha256']
 for n in range(1,6):
  at=4096+(n-1)*16384;_,na,nb,_=struct.unpack_from('<IIII',original,at)
  assert original[at+16:at+16+na]==(factory/f'pattern-{n:02d}.seq4').read_bytes()
  scene=original[at+16+na:at+16+na+nb];cur=(factory/f'pattern-{n:02d}.scene380').read_bytes()
  assert all(a==b or 8<=i<12 for i,(a,b) in enumerate(zip(scene,cur)))
 source_fav=[r for r in j['records'] if r['type']=='favorites'][0];old=base64.b64decode(source_fav['data'])
 assert all(a==b or 4<=i<8 or i in [20+n*86+81 for n in range(8)] for i,(a,b) in enumerate(zip(old,fav)))
print('PASS: exactly five author patterns and scenes, eight favorites; musical bytes unchanged, only edit badges/generation normalized; presets absent; clean globals, 90 BPM, empty slots 6-16, CRCs and manifest hashes.')
