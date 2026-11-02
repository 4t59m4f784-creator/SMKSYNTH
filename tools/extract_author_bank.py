#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Validate a B1 authoring file and extract reusable logical records (no firmware writes)."""
import argparse,base64,hashlib,json,re,struct,zlib
from pathlib import Path

def extract(file,out):
 j=json.loads(Path(file).read_text());assert j['format']=='smksynth-authoring-backup' and j['version']==1 and j['layout']=='SMK16P1' and j['model']=='SMK-37 Pro','unsupported container'
 a=base64.b64decode(j['bank'],validate=True);assert len(a)==0x46000 and a[:16]==b'SMK37DATASMK16P1','unknown bank'
 assert hashlib.sha256(a).hexdigest()==j['sha256'],'SHA256 mismatch'
 records=[];warnings=[]
 def sector(o,lengths):
  b=a[o:o+4096]
  if b==b'\xff'*4096:return None
  magic,n,crc=struct.unpack_from('<III',b);assert magic==0x59474e44 and n in lengths and zlib.crc32(b[12:12+n])==crc,f'bad sector {o:x}'
  return b[12:12+n]
 def add(path,b,**info):records.append(dict(path=path,data=b,sha256=hashlib.sha256(b).hexdigest(),**info))
 g=sector(0x41000,[67])
 if g:add('globals/set7.bin',g,type='globals')
 for bank in range(2):
  p=sector(0x42000+bank*4096,[4056])
  if p is None:continue
  assert struct.unpack_from('<I',p)[0]==0x50564f42,'bad preset tag'
  for n in range(50):
   slot=bank*50+n
   if slot>=99:break
   assert p[4+n] in [0,1],'bad occupancy'
   if p[4+n]:
    patch=p[56+n*80:136+n*80];name=patch[63:73].decode('ascii',errors='replace').strip();add(f'presets/{slot+1:02d}.patch80',patch,type='preset',slot=slot+1,name=name)
 for slot in range(16):
  b=a[4096+slot*16384:4096+(slot+1)*16384]
  if b==b'\xff'*16384:continue
  magic,n,m,crc=struct.unpack_from('<IIII',b);assert magic==0x544f4c53 and b[16:20]==b'SEQ4' and n in [13416,14448,14460,14476] and m==380 and zlib.crc32(b[16:16+n+m])==crc,f'bad pattern {slot+1}'
  add(f'patterns/{slot+1:02d}.seq4',b[16:16+n],type='pattern',slot=slot+1,prefix=n);add(f'patterns/{slot+1:02d}.scene380',b[16+n:16+n+m],type='scene',slot=slot+1)
  scene=b[16+n:16+n+m]
  if scene[:4]==b'PSN2':
   for t in range(4):add(f'patterns/{slot+1:02d}-'+chr(65+t)+'.patch80',scene[12+t*80:92+t*80],type='scene-preset',part=chr(65+t))
 candidates=[]
 for bank in range(2):
  try:
   p=sector(0x44000+bank*4096,[700,708])
   if p:
    tag,gen,used=struct.unpack_from('<III',p);assert tag==(0x32564650 if len(p)==708 else 0x31564650) and not used&~255; candidates.append((gen,p))
  except AssertionError as e:warnings.append(str(e))
 assert candidates or not warnings,'no valid favorites copy'
 if candidates:
  gen,p=candidates[0]
  for g,q in candidates[1:]:
   if 0<((g-gen)&0xffffffff)<0x80000000:gen,p=g,q
  add('favorites/bank.bin',p,type='favorites',schema='pfv2' if len(p)==708 else 'pfv1')
  used=struct.unpack_from('<I',p,8)[0];stride=86 if len(p)==708 else 85
  for n in range(8):
   if used>>n&1:add(f'favorites/{n+1}.patch80',p[20+n*stride:100+n*stride],type='favorite-preset',slot=n+1)
 # Validate everything before creating output. Never execute file contents or trust supplied paths.
 out=Path(out);assert not out.exists() or not any(out.iterdir()),'output directory must be empty'
 out.mkdir(parents=True,exist_ok=True)
 for r in records:
  p=out/r['path'];p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(r.pop('data'))
 (out/'bank-SMK16P1.bin').write_bytes(a)
 result=dict(source_sha256=j['sha256'],source_firmware=j.get('firmware'),layout='SMK16P1',records=records,warnings=warnings)
 (out/'manifest.json').write_text(json.dumps(result,indent=2,ensure_ascii=False)+'\n');return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('file');p.add_argument('output');a=p.parse_args();r=extract(a.file,a.output);print(json.dumps(dict(records=len(r['records']),warnings=r['warnings'])))
