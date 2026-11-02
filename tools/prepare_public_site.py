#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Assemble the review website from retained sources. Does not publish or build firmware."""
from pathlib import Path
import shutil,zipfile,re,json
ROOT=Path(__file__).resolve().parents[1]
SITE=ROOT/'site'
def copy(source,target):
 target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,target)
def main():
 if SITE.exists():shutil.rmtree(SITE)
 SITE.mkdir()
 for name in ['index.html','style.css','review.html']:copy(ROOT/'publication'/name,SITE/name)
 for folder in ['keyboard-b2','backup-b2','backup-b1']:
  dest=SITE/folder;dest.mkdir()
  for path in (ROOT/'web'/folder).iterdir():
   if path.is_file() and path.suffix in ['.js','.css','.html','.json','.md','.txt']:
    copy(path,dest/path.name)
  if folder=='keyboard-b2':
   shutil.copytree(ROOT/'web'/folder/'firmware',dest/'firmware')
 for name in ['index.html','app.js','engine.js','worker.js','worklet.js','style.css','build.json','obervave-voice.wasm','PADS-OCTAVE-P16O1.md','PATTERNS16-P16.md','SAVE-DISPLAY-P16F1.md','SPLIT-LAYER-SL2.md','STORAGE-COMPATIBILITY.md']:
  copy(ROOT/'web/voice-lab'/name,SITE/'simulator'/name)
 p=SITE/'simulator/index.html';s=p.read_text()
 s=s.replace('Protótipo independente da AD1 oficial.','Base musical P16.2 aprovada do SMKSYNTH.')
 s=s.replace('Simulador experimental privado · SMKTRON e instaladores preservados','Simulador musical P16.2 · Banco local separado do teclado')
 s=s.replace('SPLIT/LAYER implementados neste protótipo','SPLIT/LAYER implementados nesta base')
 s=s.replace('inclui retorno ao Player PF12/PF9. Esta é uma build experimental; a validação física está pendente.','A base musical P16.2 foi aprovada no teclado. O instalador B2 atualiza apenas instalações SMKSYNTH compatíveis e não oferece retorno ao Player nesta página.')
 s=s.replace('../keyboard-p16/','../keyboard-b2/').replace('Instalador P16.2 experimental','Atualizador SMKSYNTH B2')
 s=s.replace('REC usa os oito pads superiores como instrumentos.','REC preserva o banco manual; SEQ ativa o uso dos pads do sequenciador.')
 p.write_text(s)
 # Retain provenance, distinguish musical simulator from USB firmware and keep local-session keys unchanged.
 p=SITE/'simulator/build.json';j=json.loads(p.read_text());j['status']='P16.2 approved musical base; B2 USB not simulated';j['hardware_tested']=True;j['keyboard_installer']='../keyboard-b2/';p.write_text(json.dumps(j,indent=2)+'\n')
 p=SITE/'backup-b1/app.js';p.write_text(p.read_text().replace('../keyboard-b1/protocol.js','../keyboard-b2/protocol.js'))
 p=SITE/'backup-b1/index.html';s=p.read_text().replace('../keyboard-b1/','../keyboard-b2/').replace('Instalador B1 e limites da transição','Atualizador B2 e compatibilidade').replace('Guarde uma cópia no computador e envie o arquivo nesta conversa para prepararmos a biblioteca da distribuição pública.','Guarde uma cópia no computador.');p.write_text(s)
 for rel in ['LICENSE','LICENSING.md','CREDITS.md']:
  copy(ROOT/rel,SITE/('LICENSE.txt' if rel=='LICENSE' else rel))
 copy(ROOT/'LICENSING.md',SITE/'keyboard-b2/LICENSING.md')
 # A single corresponding-source archive: no bank, builds, local Git, site recursion or old artifacts.
 with zipfile.ZipFile(SITE/'source.zip','w',zipfile.ZIP_DEFLATED) as z:
  for path in sorted(ROOT.rglob('*')):
   rel=path.relative_to(ROOT)
   if not path.is_file() or any(x in {'.git','.openai','site','build','__pycache__'} for x in rel.parts):continue
   if path.suffix in {'.zip','.smksynth','.fwsc','.wasm','.wav','.mp4','.pyc'}:continue
   z.write(path,Path('SMKSYNTH')/rel)
 # Explicitly include B2/p16.2 sources once; binaries stay in their runtime locations.
 for folder in ['keyboard-b2','simulator']:copy(SITE/'source.zip',SITE/folder/'source.zip')
 (SITE/'.nojekyll').touch()
 print(f'Prepared {SITE}')
if __name__=='__main__':main()
