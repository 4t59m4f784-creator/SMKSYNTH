#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Check shipped review links/assets and preserve known B2/P16.2 binaries."""
from pathlib import Path
from html.parser import HTMLParser
from urllib.parse import urlparse,unquote
import hashlib,json,re,zipfile
ROOT=Path(__file__).resolve().parents[1];SITE=ROOT/'site'
class Links(HTMLParser):
 def __init__(self):super().__init__();self.links=[]
 def handle_starttag(self,tag,attrs):
  self.links += [v for k,v in attrs if k in ['href','src'] and v]
def main():
 errors=[]
 def target(path,link):
  u=urlparse(link)
  if u.scheme or u.netloc or not u.path:return
  if u.path.startswith('/'):errors.append(f'Root-absolute URL breaks project subpath: {path}: {link}');return
  dest=path.parent/unquote(u.path)
  if not dest.exists():errors.append(f'Missing dependency: {path.relative_to(ROOT)}: {link}')
 for p in SITE.rglob('*.html'):
  parser=Links();parser.feed(p.read_text())
  for link in parser.links:target(p,link)
 for p in SITE.rglob('*.js'):
  for link in re.findall(r'''(?:from\s*|fetch\(|addModule\(|new URL\()\s*['"]([^'"]+)['"]''',p.read_text()):
   if link.startswith('.') or not urlparse(link).scheme:target(p,link)
 fw=SITE/'keyboard-b2/firmware/obervave-3.0.1-b2-smk37.fwsc'
 assert hashlib.sha256(fw.read_bytes()).hexdigest()=='50054674cc3db57c971321b9f94993099f1a5505125917dd23400588914a5685'
 assert hashlib.sha256((SITE/'simulator/obervave-voice.wasm').read_bytes()).hexdigest()=='a37f905483ed2e52271a38ceb86f0fe4b76120865e68e69dd757bfc2990164bb'
 for p in SITE.rglob('*'):
  if p.is_file() and p.suffix in {'.smksynth','.wav','.mp4'}:errors.append(f'Unexpected personal/media file: {p}')
 with zipfile.ZipFile(SITE/'source.zip') as z:
  names=z.namelist()
  assert 'SMKSYNTH/firmware/src/obervave.c' in names
  assert 'SMKSYNTH/firmware/loader/ldr_core.c' in names
  assert 'SMKSYNTH/LICENSE' in names
  assert not any(n.endswith('.smksynth') or '/.git/' in n or '/.openai/' in n for n in names)
 if errors:raise SystemExit('\n'.join(errors))
 print('PASS: local HTML/module links, project-subpath paths, exact B2 firmware, exact approved simulator, source archive and no bank files.')
if __name__=='__main__':main()
