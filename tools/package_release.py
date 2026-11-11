#!/usr/bin/env python3
"""Package the version in RELEASE.json and assemble the GitHub Pages website."""
from pathlib import Path
import argparse,hashlib,json,shutil,subprocess,zipfile
ROOT=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def package(destination,reuse_release=False):
 info=json.loads((ROOT/'RELEASE.json').read_text());version=info['version'];out=ROOT/'releases'/version;out.mkdir(parents=True,exist_ok=True)
 archive=out/f'SMKSYNTH-{version}-source.zip'
 if not reuse_release:
  with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
   names=subprocess.check_output(['git','ls-files','--cached','--others','--exclude-standard'],cwd=ROOT,text=True).splitlines()
   for name in sorted(set(names)):
    p=ROOT/name;rel=Path(name)
    if not p.is_file() or any(x in rel.parts for x in ['.git','build','site','experimental','validation','releases','node_modules','__pycache__']):continue
    if p.suffix in ['.zip','.fwsc','.pcm','.smksynth','.wav','.mp4','.pyc','.gz']:continue
    if p.suffix in ['.wasm','.bin'] and not str(rel).startswith(('factory/','web/simulator/')):continue
    if name in ['.github/workflows/physical-controls.yml','tools/check_controls_candidate.py','tools/package_controls_candidate.py']:continue
    z.write(p,f'SMKSYNTH-{version}-source/'+name)
  shutil.copy2(ROOT/'RELEASE.json',out/'RELEASE.json')
  assets=sorted(p for p in out.iterdir() if p.is_file() and p.name!='SHA256SUMS.txt')
  (out/'SHA256SUMS.txt').write_text(''.join(sha(p)+'  '+p.name+'\n' for p in assets))
 dest=Path(destination);dest.mkdir(parents=True,exist_ok=True)
 for name in ['index.html','style.css','i18n.js']:shutil.copy2(ROOT/'web/site'/name,dest/name)
 for source,name in [(ROOT/'web/install','install'),(ROOT/'web/install-elite','install-elite'),(ROOT/'web/backup','backup'),(ROOT/'web/simulator','simulator'),(ROOT/'docs','docs'),(ROOT/'docs/images','assets'),(out,'downloads')]:shutil.copytree(source,dest/name,dirs_exist_ok=True)
 for name in ['README.md','LICENSE','LICENSING.md','CREDITS.md','CHANGELOG.md','RELEASE.json']:shutil.copy2(ROOT/name,dest/name)
 print(json.dumps(dict(destination=str(dest),version=version,source_sha256=sha(archive))))
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('destination');p.add_argument('--reuse-release',action='store_true');a=p.parse_args();package(a.destination,a.reuse_release)
