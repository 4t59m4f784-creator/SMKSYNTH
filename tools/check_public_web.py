#!/usr/bin/env python3
"""Check the assembled public release, without rebuilding firmware or changing banks."""
import hashlib
import json
from html.parser import HTMLParser
from pathlib import Path
import re
import sys
from urllib.parse import unquote, urlparse

ROOT = Path(__file__).resolve().parents[1]


class Links(HTMLParser):
    def __init__(self):
        super().__init__()
        self.links = []

    def handle_starttag(self, tag, attrs):
        self.links.extend(v for k, v in attrs if k in ('href', 'src') and v)


def check(site):
    site = site.resolve()
    release = json.loads((ROOT / 'RELEASE.json').read_text())
    version = release['version']

    def digest(path):
        return hashlib.sha256(path.read_bytes()).hexdigest()

    def dependency(page, link):
        url = urlparse(link)
        if url.scheme or url.netloc or not url.path:
            return
        assert not url.path.startswith('/'), f'Project-root URL: {page}: {link}'
        target = (page.parent / unquote(url.path)).resolve()
        assert target.is_relative_to(site), f'Outside public site: {page}: {link}'
        assert target.exists(), f'Missing public dependency: {page}: {link}'

    for folder in ('', 'install', 'backup', 'simulator'):
        page = site / folder / 'index.html'
        source = ROOT / 'web' / (folder or 'site') / 'index.html'
        assert page.read_bytes() == source.read_bytes(), f'Stale public page: {folder or "home"}'
        html = page.read_text()
        assert 'i18n.js' in html, f'Missing language support: {page}'
        parser = Links()
        parser.feed(html)
        for link in parser.links:
            dependency(page, link)
            if 'docs/' in link and link.endswith('.md') and not link.endswith('.en.md'):
                dependency(page, link[:-3] + '.en.md')

    parser = Links()
    elite = site / 'install-elite/index.html'
    parser.feed(elite.read_text())
    assert 'language.js' in (elite.parent / 'controller.js').read_text()
    for link in parser.links:
        dependency(elite, link)

    for folder in ('install', 'install-elite', 'backup', 'simulator'):
        for script in (site / folder).glob('*.js'):
            for link in re.findall(r'''(?:from\s*|fetch\(|addModule\(|new URL\()\s*['"]([^'"]+)['"]''', script.read_text()):
                dependency(script, link)

    assert (site / 'i18n.js').read_bytes() == (ROOT / 'web/site/i18n.js').read_bytes()
    pins = json.loads((site / 'install/manifest.json').read_text())
    assert set(pins)=={'pro','elite'}
    for model, config in pins.items():
        for kind in ['release','update']:
            pin=config[kind]
            assert version in pin['name'], f'Wrong release label: {model}/{kind}'
            assert pin['product'].startswith(config['model']+'_')
            path=site/'install'/pin['url']
            assert path.stat().st_size==pin['size'] and digest(path)==pin['sha'], f'Invalid package: {model}/{kind}'
    assert digest(site / 'simulator/obervave-voice.wasm') == release['wasm_sha256'], 'Changed simulator WASM'
    assert (site / 'simulator/factory-session.bin').read_bytes() == (ROOT / 'factory/simulator-session.bin').read_bytes(), 'Changed factory session'
    for language in ('PT', 'EN'):
        for kind in ('Quick-Start', 'Manual'):
            pdf = site / 'docs/pdf' / f'SMKSYNTH-{version}-{kind}-{language}.pdf'
            assert pdf.read_bytes().startswith(b'%PDF-'), f'Missing PDF: {pdf}'
    assert (site / 'downloads' / f'SMKSYNTH-{version}-source.zip').is_file()
    print(f'PASS: public {version} pages, PT/EN links, modules, firmware pins, frozen simulator and PDFs')


if __name__ == '__main__':
    check(Path(sys.argv[1] if len(sys.argv) > 1 else 'build/public-site'))
