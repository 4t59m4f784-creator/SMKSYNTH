#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Assemble the ML1 test installer/simulator without changing public 1.2.0 assets."""
from pathlib import Path
import shutil,json,hashlib,subprocess,zipfile,re
R=Path(__file__).resolve().parents[1];out=R/'experimental/midi-layer';sim=out/'simulator';sim.mkdir(exist_ok=True)
for n in ['index.html','app.js','engine.js','worker.js','worklet.js','style.css','midi.js','factory-session.bin']:
 src=R/'web/simulator'/n
 if src.exists():shutil.copy2(src,sim/n)
for p in sim.glob('*'):
 if p.suffix in ['.html','.js']:
  s=p.read_text().replace('1.2.0','1.2.0-ml1').replace('smksynth-1.0.0','smksynth-midi-layer-ml1').replace('../install/','../').replace('../docs/','https://4t59m4f784-creator.github.io/SMKSYNTH/docs/');s=re.sub(r'\?v=[A-Za-z0-9.-]+','?v=ml1',s);p.write_text(s)
shutil.copy2(R/'build/ml1-sim/SMKSYNTH-1.2.0-ml1.wasm',sim/'obervave-voice.wasm')
shutil.copy2(R/'web/site/i18n.js',out/'i18n.js')
for n in ['LICENSE','LICENSING.md','CREDITS.md']:shutil.copy2(R/n,out/n)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
meta={'version':'1.2.0-ml1','identity':19650,'official_base':'1.2.0 /19649','native_asan_ubsan':'passed','max_tested_keys':37,'max_tested_channels':16,'static_ram_bytes':95648,'static_ram_limit':98304,'pool_bytes':328368,'pool_limit':344064,'wasm_sha256':sha(sim/'obervave-voice.wasm'),'hardware_tested':False}
(out/'BUILD.json').write_text(json.dumps(meta,indent=2)+'\n')
names=subprocess.check_output(['git','ls-files','--cached','--others','--exclude-standard'],cwd=R,text=True).splitlines();archive=out/'source.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
 for name in sorted(set(names)):
  p=R/name;parts=p.relative_to(R).parts
  if not p.is_file() or any(x in parts for x in ['.git','build','releases','node_modules','__pycache__']):continue
  if parts[0]=='experimental' and parts[:2]!=('experimental','midi-layer'):continue
  if p.name=='SHA256SUMS.txt' or p.suffix in ['.zip','.fwsc','.pcm','.wav','.mp4','.gz','.pyc']:continue
  if p.suffix in ['.wasm','.bin'] and not (name.startswith('factory/') or name=='experimental/midi-layer/simulator/factory-session.bin'):continue
  info=zipfile.ZipInfo('SMKSYNTH-ML1-source/'+name,date_time=(2026,11,14,0,0,0));info.create_system=3;info.external_attr=(0o100755 if p.stat().st_mode&0o111 else 0o100644)<<16;z.writestr(info,p.read_bytes(),compress_type=zipfile.ZIP_DEFLATED,compresslevel=9)
assets=list(out.glob('*.fwsc'))+[archive,sim/'obervave-voice.wasm'];(out/'SHA256SUMS.txt').write_text(''.join(sha(p)+'  '+str(p.relative_to(out))+'\n' for p in sorted(assets)))
print(json.dumps({'directory':str(out),'source_sha256':sha(archive),'wasm_sha256':meta['wasm_sha256']}))
