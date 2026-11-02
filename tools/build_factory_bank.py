#!/usr/bin/env python3
"""Assemble only approved factory records. No arbitrary author bank is bundled."""
import argparse,hashlib,json,struct,zlib
from pathlib import Path

def build(source,output):
 source=Path(source);output=Path(output)
 bank=bytearray(b'\xff'*0x46000);bank[:16]=b'SMK37DATASMK16P1'
 session=bytearray(19*16384)
 def sector(at,data):bank[at:at+12+len(data)]=struct.pack('<III',0x59474e44,len(data),zlib.crc32(data))+data
 globals_=(output/'globals-set7.bin').read_bytes();assert len(globals_)==67 and globals_[0]==7
 sector(0x41000,globals_);session[0]=1;session[4:4+len(globals_)]=globals_
 names=[]
 for n in range(1,6):
  path=source/f'pattern-{n:02d}.seq4'
  if not path.exists():path=source/f'patterns/{n:02d}.seq4'
  pattern=path.read_bytes();assert len(pattern) in [13416,14448,14460,14476] and pattern[:4]==b'SEQ4'
  scene=(output/f'pattern-{n:02d}.scene380').read_bytes();assert len(scene)==380 and scene[:4]==b'PSN2'
  # Selected snapshots are standalone; program metadata never imports user presets.
  (output/f'pattern-{n:02d}.seq4').write_bytes(pattern)
  at=4096+(n-1)*16384;data=pattern+scene
  bank[at:at+16+len(data)]=struct.pack('<IIII',0x544f4c53,len(pattern),len(scene),zlib.crc32(data))+data
  at=n*16384;session[at]=1;struct.pack_into('<II',session,at+4,len(pattern),len(scene));session[at+16:at+16+len(data)]=data
  names.append(dict(slot=n,parts=[scene[12+t*80+63:12+t*80+73].decode('ascii','replace').strip() for t in range(4)]))
 fav=(output/'favorites-pfv2.bin').read_bytes();assert len(fav)==708 and fav[:4]==b'PFV2' and struct.unpack_from('<I',fav,8)[0]==255
 sector(0x44000,fav);struct.pack_into('<I',session,17*16384,len(fav));session[17*16384+4:17*16384+4+len(fav)]=fav
 assert bank[0x42000:0x44000]==b'\xff'*8192
 assert bank[4096+5*16384:0x41000]==b'\xff'*(11*16384)
 (output/'bank-SMK16P1.bin').write_bytes(bank);(output/'simulator-session.bin').write_bytes(session)
 files={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(output.iterdir()) if p.is_file() and p.name!='manifest.json'}
 result=dict(format='smksynth-curated-factory-v1',patterns=names,empty_patterns=list(range(6,17)),favorites=[dict(slot=n+1,name=fav[20+n*86+63:20+n*86+73].decode('ascii','replace').strip()) for n in range(8)],user_presets=0,tempo=globals_[2],files=files)
 (output/'manifest.json').write_text(json.dumps(result,indent=2,ensure_ascii=False)+'\n');return result

if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('source');p.add_argument('output');a=p.parse_args();r=build(a.source,a.output);print(json.dumps({k:r[k] for k in ['patterns','favorites','user_presets','tempo']},ensure_ascii=False))
