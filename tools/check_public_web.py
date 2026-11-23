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
        self.meta = {}

    def handle_starttag(self, tag, attrs):
        self.links.extend(v for k, v in attrs if k in ('href', 'src') and v)
        if tag=='meta':
            a=dict(attrs)
            self.meta[a.get('property',a.get('name'))]=a.get('content')


class InstallerLayout(HTMLParser):
    def __init__(self):
        super().__init__()
        self.boxes = []
        self.summaries = []

    def handle_starttag(self, tag, attrs):
        a = dict(attrs)
        if tag == 'section' or 'guide-box' in a.get('class', '').split() or a.get('id') in {'novidades-versao', 'preparacao', 'instalar', 'depois-instalacao', 'recuperacao'}:
            self.boxes.append((a.get('id'), tag, 'open' in a))
        if tag == 'summary':
            self.summaries.append(a)


def check_installer_layout(html, release):
    layout = InstallerLayout()
    layout.feed(html)
    assert layout.boxes == [
        ('novidades-versao', 'section', False),
        ('preparacao', 'details', False),
        ('instalar', 'section', False),
        ('depois-instalacao', 'details', False),
        ('recuperacao', 'details', False),
    ], 'Installer order, fixed boxes or default-collapsed sections changed; see AGENTS.md'
    assert all(a.get('data-pt') and a.get('data-en') for a in layout.summaries[:3]), 'Guide summaries need PT/EN labels'
    highlights = re.search(r'<section[^>]*id="novidades-versao".*?</section>', html, re.S).group()
    assert release['version'] in highlights and release['edition'] in highlights, 'Release box must show current version and edition'


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
        if folder == 'install':
            check_installer_layout(html, release)
        parser = Links()
        parser.feed(html)
        if folder in ('', 'install'):
            origin='https://4t59m4f784-creator.github.io/SMKSYNTH/'
            assert parser.meta['og:url']==origin+('install/' if folder else '')
            assert parser.meta['og:image']==origin+'assets/smksynth-social-v3.png'
            assert parser.meta['og:image:width']=='1200' and parser.meta['og:image:height']=='630'
            assert parser.meta['twitter:card']=='summary_large_image'
            card=(site/'assets/smksynth-social-v3.png').read_bytes()
            assert card[:8]==b'\x89PNG\r\n\x1a\n' and int.from_bytes(card[16:20],'big')==1200 and int.from_bytes(card[20:24],'big')==630
        if folder == '':
            embeds = re.findall(r'youtube(?:-nocookie)?\.com/embed/([A-Za-z0-9_-]+)', html)
            watches = re.findall(r'youtube\.com/watch\?v=([A-Za-z0-9_-]+)', html)
            assert embeds and watches and set(watches) <= set(embeds), 'Demo video link differs from embedded video'
        for link in parser.links:
            dependency(page, link)
            if 'docs/' in link and link.endswith('.md') and not link.endswith('.en.md'):
                dependency(page, link[:-3] + '.en.md')

    # Archived 1.2.0 must keep working for interrupted installations.
    legacy = site / 'install/legacy-1-2-0/index.html'
    parser = Links()
    parser.feed(legacy.read_text())
    for link in parser.links:
        dependency(legacy, link)

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
