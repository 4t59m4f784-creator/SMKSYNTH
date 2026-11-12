#!/usr/bin/env python3
"""Build the branded social card as SVG and PNG (requires Inkscape)."""
from pathlib import Path
import base64,subprocess
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
image=base64.b64encode((ROOT/'docs/images/sequence.png').read_bytes()).decode()
pads=''.join(f'<rect x="{72+c*66}" y="{466+r*43}" width="54" height="32" rx="5" fill="{["#234461","#452536"][r]}" stroke="{["#78b6ed","#f0474d"][r]}" stroke-opacity=".6"/>' for r in range(2) for c in range(8))
svg=f'''<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="1200" height="630" viewBox="0 0 1200 630">
<defs><linearGradient id="bg" x2="1" y2="1"><stop stop-color="#101c2b"/><stop offset="1" stop-color="#080d15"/></linearGradient><linearGradient id="line"><stop stop-color="#f0474d"/><stop offset="1" stop-color="#78b6ed"/></linearGradient></defs>
<rect width="1200" height="630" fill="url(#bg)"/><rect width="1200" height="7" fill="url(#line)"/>
<g font-family="DejaVu Sans,sans-serif"><text x="72" y="83" font-size="20" letter-spacing="3" fill="#8cbce8">M-VAVE SMK-37 PRO + ELITE</text>
<text x="66" y="190" font-weight="bold" font-size="84" letter-spacing="-3" fill="#edf1f5">SMK<tspan fill="#f0474d">SYNTH</tspan></text>
<text x="72" y="277" font-size="43" font-weight="bold" fill="#edf1f5">Um synth completo.</text><text x="72" y="331" font-size="43" font-weight="bold" fill="#edf1f5">No seu SMK-37.</text>
<text x="72" y="391" font-size="23" fill="#a6bdd1">4 partes · Split &amp; Layer · Sequenciador</text><text x="72" y="425" font-size="23" fill="#a6bdd1">MIDI USB/TRS · Sustain · MIDI Panic</text>
{pads}
<rect x="754" y="124" width="374" height="416" rx="22" fill="#060b12" stroke="#324b65" stroke-width="2"/>
<circle cx="779" cy="154" r="5" fill="#f0474d"/><text x="795" y="160" font-size="14" letter-spacing="1.8" fill="#9bb5cc">SMKSYNTH / SEQUENCER</text>
<image x="774" y="183" width="334" height="334" style="image-rendering:pixelated" xlink:href="data:image/png;base64,{image}"/>
<path d="M72 561 H1128" stroke="#2a3c50"/>
<text x="72" y="599" font-size="19" fill="#9fb3c6">Firmware gratuito e de código aberto</text><text x="1128" y="599" text-anchor="end" font-size="18" fill="#e5edf5">Por Marcus Padrini</text></g></svg>'''
out=ROOT/'docs/images/smksynth-social-v1.svg';out.write_text(svg)
subprocess.run(['inkscape',str(out),'--export-type=png','--export-filename='+str(out.with_suffix('.png'))],check=True)

# Palette encoding keeps the crawler download small without changing the SVG artwork.
png=out.with_suffix(".png")
with Image.open(png) as im:
    im.convert("RGB").quantize(colors=256).save(png,optimize=True)
