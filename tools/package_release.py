#!/usr/bin/env python3
"""Build the isolated review website and corresponding-source distribution."""
from pathlib import Path
import argparse,hashlib,json,shutil,zipfile
ROOT=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def package(destination):
 out=ROOT/'releases/1.0.0';out.mkdir(parents=True,exist_ok=True)
 fw=ROOT/'web/install/firmware/SMKSYNTH-1.0.0-SMK37-Pro.fwsc';wasm=ROOT/'web/simulator/obervave-voice.wasm'
 info=dict(version='1.0.0',identity='SMK-37 Pro_19618',required_origin='SMK-37 Pro_016',installation='first-install-replaces-bank',runtime_base='6c11279e6bfcfb627dfb54628dddc2ee8a653f58',factory_patterns=5,favorites=8,extra_user_presets=0,tempo=90,dp2_runtime_hardware_approved=True,first_install_hardware_approved=False,firmware_sha256=sha(fw),wasm_sha256=sha(wasm),factory_bank_sha256=sha(ROOT/'factory/bank-SMK16P1.bin'),app_bytes=161660,static_ram_bytes=82896,pool_bytes=325984,loader_bytes=7211)
 (ROOT/'RELEASE.json').write_text(json.dumps(info,indent=2)+'\n');shutil.copy2(fw,out/fw.name)
 archive=out/'SMKSYNTH-1.0.0-source.zip'
 with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
  for p in sorted(ROOT.rglob('*')):
   if not p.is_file():continue
   rel=p.relative_to(ROOT)
   if str(rel)=='validation/release-site-package.log':continue
   if any(x in rel.parts for x in ['.git','build','dist','releases','node_modules','__pycache__']):continue
   if p.suffix in ['.zip','.fwsc','.pcm','.smksynth']:continue
   # Curated factory and final simulator bytes are distribution sources/data;
   # unrelated checkpoint binaries are not needed in a source archive.
   if p.suffix in ['.wasm','.bin'] and not str(rel).startswith(('factory/','web/simulator/')):continue
   z.write(p,'SMKSYNTH-1.0.0-source/'+str(rel))
 (out/'SHA256SUMS.txt').write_text('\n'.join(sha(p)+'  '+p.name for p in [out/fw.name,archive])+'\n')
 shutil.copy2(ROOT/'RELEASE.json',out/'RELEASE.json')
 dest=Path(destination);dest.mkdir(parents=True,exist_ok=True)
 shutil.copy2(ROOT/'web/site/index.html',dest/'index.html');shutil.copy2(ROOT/'web/site/style.css',dest/'style.css')
 for source,name in [(ROOT/'web/install','install'),(ROOT/'web/backup','backup'),(ROOT/'web/simulator','simulator'),(ROOT/'docs','docs'),(ROOT/'docs/images','assets'),(out,'downloads')]:shutil.copytree(source,dest/name,dirs_exist_ok=True)
 for name in ['README.md','LICENSE','LICENSING.md','CREDITS.md','CHANGELOG.md','RELEASE.json']:shutil.copy2(ROOT/name,dest/name)
 print(json.dumps(dict(destination=str(dest),firmware_sha256=info['firmware_sha256'],source_sha256=sha(archive),source_bytes=archive.stat().st_size)))
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('destination');a=p.parse_args();package(a.destination)
