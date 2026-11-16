#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Package TF1 locally without modifying the official release or public installer."""
import hashlib,json,re,shutil,zipfile
from pathlib import Path
R=Path(__file__).resolve().parents[1]
D=R/'build/SMKSYNTH-TF1-Teste'
D.mkdir(parents=True,exist_ok=True)
old=R/'experimental/midi-layer'
for name in ['controller.js','protocol.js','package.js','style.css']:
    shutil.copy2(old/name,D/name)
pins={}
for model in ['pro','elite']:
    label='Pro' if model=='pro' else 'Elite'
    suffix='' if model=='pro' else '-elite'
    for kind in ['update','release']:
        target=f'SMKSYNTH-{("1.2.1-TF1" if kind=="update" else "1.2.1-oficial")}-{label}.fwsc'
        source=R/f'build/obervave-1.2.1-tf1-smk37{suffix}.fwsc' if kind=='update' else R/f'web/install/firmware/SMKSYNTH-1.2.1-update-SMK37-{label}.fwsc'
        shutil.copy2(source,D/target)
        raw=(D/target).read_bytes()
        pins[model+'.'+kind]=dict(name=('Instalar TF1' if kind=='update' else 'Retornar à oficial 1.2.1')+' · '+label,product=f'SMK-37 {label}_{19654 if kind=="update" else 19653}',url='./'+target,size=len(raw),sha=hashlib.sha256(raw).hexdigest())
s=(old/'service.js').read_text()
s=re.sub(r'export const PINS=.*?;\n','export const PINS=Object.freeze('+json.dumps(pins,ensure_ascii=False)+');\n',s,count=1)
s=s.replace('smksynth.midi.ema.pending.v1','smksynth.tuning.tf1.pending.v1')
public=(R/'web/install/service.js').read_text()
arrays={n:json.loads(re.search(r'const '+n+r'=(\[.*?\]);',public)[1])+['19654'] for n in ['PRO','ELITE']}
a="const sourceAllowed=(kind,text)=>{const m=/^(SMK-37 Pro|SMK-37 Elite)_(\\d+)$/.exec(text||'');return !!m&&(m[1]==='SMK-37 Pro'?"+json.dumps(arrays['PRO'])+':'+json.dumps(arrays['ELITE'])+").includes(m[2]);};"
s=s[:s.index('const sourceAllowed=')]+a+'\n'+s[s.index('export function readPending'):]
s=s.replace('Abra este instalador em HTTPS','Abra este instalador em localhost ou HTTPS')
(D/'service.js').write_text(s)
c=(D/'controller.js').read_text().replace('v=ema','v=tf1').replace('Endless MIDI A /19652','TF1 /19654').replace('oficial 1.2.0 /19649','oficial 1.2.1 /19653').replace('banco B seleciona os canais do teclado; pads simultâneos formam layers.','corrige a afinação após retornar do MODO MIDI e usar MIDI PANIC.').replace('em HTTPS.','em localhost ou HTTPS.').replace('smksynth-ema-installer-log','smksynth-tf1-installer-log')
(D/'controller.js').write_text(c)
(D/'index.html').write_text('''<!doctype html><html lang="pt-BR"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>SMKSYNTH TF1 · Teste</title><link rel="stylesheet" href="style.css"><main><header><p>SMKSYNTH · PRO / ELITE · EXPERIMENTAL</p><h1>Endless MIDI <span>TF1</span></h1><p>1.2.1-tf1 · 19654 · Correção de afinação ao retornar do MODO MIDI e após MIDI PANIC.</p></header><section><h2>Novidades da versão</h2><p>Preserva o gerador aleatório das vozes durante a limpeza. Evita a queda progressiva de afinação em timbres com Vintage e preserva o ruído dos osciladores.</p></section><details><summary>1 · Antes de começar</summary><p>Somente para teclados que já usam SMKSYNTH. Exporte seu backup. Feche DAWs, outros instaladores e aplicativos MIDI. Use Chrome ou Edge no computador. Mantenha este servidor aberto.</p><p>Durante a gravação, não remova o cabo USB, não mude de aba e mantenha a alimentação.</p></details><section class="device"><h2>2 · Conecte e instale</h2><p id="packageStatus">Conferindo arquivos locais…</p><button id="connect">Conectar e consultar</button><p id="deviceStatus">Nenhum dispositivo acessado.</p><div class="actions"><button id="update" class="primary" disabled>Instalar TF1 · 19654</button><button id="release" disabled>Retornar à oficial 1.2.1 · 19653</button></div><progress id="progress" max="1" value="0"></progress><p id="operationStatus">Conecte para identificar o modelo.</p></section><details><summary>Depois da instalação · Testes</summary><p>Use um timbre com Vintage ativo. Compare antes e depois de entrar e sair do MODO MIDI várias vezes. Teste GLOBE + PLAY (MIDI PANIC), pitch bend, mod wheel, sustain e timbres com ruído.</p><p>Os programas salvos são preservados. Para voltar à oficial, use o botão desta página: o instalador público ainda não reconhece a identidade TF1.</p></details><details><summary>Problemas na instalação?</summary><p>Não inicie outro procedimento sobre uma transferência incompleta. Preserve o log e use esta mesma página para retomar a operação pendente. Esta página não instala nem restaura firmware M-VAVE V016.</p><p><a href="https://4t59m4f784-creator.github.io/SMKSYNTH/">Site oficial e orientações de recuperação</a></p></details><details><summary>Log</summary><pre id="log"></pre><button id="downloadLog">Baixar log</button></details><dialog id="confirmation"><form method="dialog"><h2 id="confirmTitle"></h2><p id="confirmDetails"></p><p>Não remova o cabo USB nem mude de aba durante a gravação.</p><div class="actions"><button value="cancel">Cancelar</button><button value="confirm" id="confirmWrite">Confirmar firmware</button></div></form></dialog><footer>SMKSYNTH · Marcus Padrini · OberVave / zvenson · JIANT · Clipper / 3dSam · Snoop37 / genkigenki · GPL-3.0-only. Projeto experimental. O autor não se responsabiliza por danos ao equipamento durante a atualização. Inclui fontes correspondentes em source.zip.</footer><script type="module" src="controller.js?v=tf1"></script></main></html>''')
(D/'start.py').write_text('''import http.server,webbrowser,os\nfrom pathlib import Path\nos.chdir(Path(__file__).resolve().parent)\nserver=http.server.ThreadingHTTPServer(("127.0.0.1",8765),http.server.SimpleHTTPRequestHandler)\nprint("Abra http://localhost:8765 no Chrome ou Edge. Mantenha esta janela aberta.")\nwebbrowser.open("http://localhost:8765")\nserver.serve_forever()\n''')
(D/'LEIA-ME.txt').write_text('''SMKSYNTH 1.2.1-tf1 / 19654 — Pro e Elite\n\nExtraia TODO o ZIP. Abra Terminal nesta pasta e execute:\nMac: python3 start.py\nWindows (Python instalado): py start.py\nAbra http://localhost:8765 no Chrome ou Edge. Não abra index.html diretamente.\nMantenha Terminal aberto até terminar. Os quatro pacotes são locais e verificados antes da gravação.\n\nExporte backup antes de instalar. Não remova USB nem mude de aba durante a gravação.\nEsta é uma ATUALIZAÇÃO de SMKSYNTH existente, não instalação inicial V016.\nUse esta mesma página para retornar à oficial 1.2.1. O instalador público ainda não aceita 19654.\n\nTeste um preset com Vintage: compare afinação antes e depois de várias entradas/saídas do MODO MIDI, e após GLOBE+PLAY. Teste mod wheel, bend, sustain e oscilador noise.\n\nA oficial não foi substituída. Testes automatizados aprovados; validação física pendente.\nNão altera formato dos programas, loader nem mapa de flash.\nFontes correspondentes GPL-3.0-only em source.zip.\n''')
with zipfile.ZipFile(D/'source.zip','w',zipfile.ZIP_DEFLATED) as z:
    for folder in ['firmware','tools','tests','docs']:
        for p in (R/folder).rglob('*'):
            if p.is_file() and '__pycache__' not in p.parts:z.write(p,p.relative_to(R))
    for name in ['LICENSE','README.md','Makefile']:
        if (R/name).exists():z.write(R/name,name)
    for p in old.iterdir():
        if p.suffix in ['.js','.css']:z.write(p,p.relative_to(R))
(D/'manifest.json').write_text(json.dumps(pins,indent=2,ensure_ascii=False))
out=R/'build/SMKSYNTH-TF1-Teste.zip'
with zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED) as z:
    for p in D.iterdir():z.write(p,D.name+'/'+p.name)
print(out)
