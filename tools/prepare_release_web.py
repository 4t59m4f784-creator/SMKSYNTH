#!/usr/bin/env python3
from pathlib import Path
import json,hashlib,shutil,re
root=Path(__file__).resolve().parents[1];web=root/'web';sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
install=web/'install';backup=web/'backup';sim=web/'simulator'
for p in [install,backup,sim]:p.mkdir(exist_ok=True)
for name in ['protocol.js','package.js','package.json','style.css','LICENSE.txt','LICENSING.md','JieLi-Apache-2.0.txt']:shutil.copy2(web/'keyboard-dp2'/name,install/name)
fw=root/'build/obervave-1.0.0-smk37.fwsc';(install/'firmware').mkdir(exist_ok=True);shutil.copy2(fw,install/'firmware/SMKSYNTH-1.0.0-SMK37-Pro.fwsc')
pins={'release':dict(name='SMKSYNTH 1.0.0 · Instalação inicial',product='SMK-37 Pro_19618',size=fw.stat().st_size,sha=sha(fw),url='./firmware/SMKSYNTH-1.0.0-SMK37-Pro.fwsc')}
(install/'manifest.json').write_text(json.dumps(pins,indent=2)+'\n')
s=(web/'keyboard-dp2/service.js').read_text();s=re.sub(r'export const PINS=.*?;\n',lambda m:'export const PINS=Object.freeze('+json.dumps(pins)+');\n',s,count=1)
s=s.replace('smksynth.dp2.pending.v1','smksynth.1.0.0.first-install.pending.v1');s=re.sub(r'const SOURCES=new Set\(.*?\);',"const SOURCES=new Set(['SMK-37 Pro_016']);",s,count=1)
s=s.replace("pending.sha===PINS[kind].sha)","pending.sha===PINS[kind].sha&&pending.source==='SMK-37 Pro_016')")
s=s.replace("Identidade não habilitada: '+id.text+'. Nenhuma gravação iniciada. Baixe o log.","Instalação inicial exige M-VAVE oficial V016 (SMK-37 Pro_016). Detectado: '+id.text+'. Nenhuma gravação iniciada.")
(install/'service.js').write_text(s)
s=(web/'keyboard-dp2/controller.js').read_text().replace('DP2 e retorno DP1 verificados','SMKSYNTH 1.0.0 verificado')
s=s.replace('Guarde um backup antes de atualizar. O banco SMK16P1 é preservado pelo loader testado; Os ajustes da DP2 ainda requerem validação física.','Esta instalação inicial substitui a área do instrumento e instala cinco patterns e oito favoritos. O banco anterior do SMKSYNTH não é preservado por este pacote. A origem deve ser M-VAVE oficial V016.')
(install/'controller.js').write_text(s)
for name in ['app.js','bank.js','client.js','file.js','index.html']:shutil.copy2(web/'backup-dp2'/name,backup/name)
shutil.copy2(web/'backup-b1/client.js',backup/'legacy-client.js')
for name in ['client.js','app.js','file.js']:
 p=backup/name;s=p.read_text().replace("'19617'","'19617','19618'").replace("'SMK-37 Pro_19617'","'SMK-37 Pro_19617','SMK-37 Pro_19618'").replace('../backup-b1/client.js','./legacy-client.js').replace('../keyboard-dp2/protocol.js','../install/protocol.js');p.write_text(s)
p=backup/'index.html';s=p.read_text().replace('Backup B1 / B2 / DP1 / DP2','SMKSYNTH — Backup e restauração').replace('SMK37 PRO · LABORATÓRIO PRIVADO · DP2 EXPERIMENTAL','SMKSYNTH · SMK-37 PRO').replace('DP2 / 19617.','DP2 / 19617 ou SMKSYNTH 1.0.0 / 19618.').replace('../keyboard-dp2/SEQUENCER-DP2.md','../docs/INSTALLATION.md').replace('../keyboard-dp2/','../install/').replace('Instalador DP2','Instalador SMKSYNTH').replace('<a href="../backup-b1/">Ferramenta B1 preservada</a> · ','').replace('A DP2 passou nos testes automatizados; USB e uso físico desta build ainda precisam de validação.','A ferramenta lê os dados salvos na memória. Salve suas alterações no instrumento antes de exportar.');p.write_text(s)
for name in ['app.js','engine.js','worker.js','worklet.js','style.css','index.html','obervave-voice.wasm']:shutil.copy2(web/'voice-lab'/name,sim/name)
shutil.copy2(root/'factory/simulator-session.bin',sim/'factory-session.bin');shutil.copy2(root/'factory/manifest.json',sim/'factory.json')
p=sim/'app.js';s=p.read_text().replace("key='smksynth-dp2'","key='smksynth-1.0.0'").replace('SMKSYNTH DP2 · DRUM PADS','SMKSYNTH 1.0.0').replace("build:'3.0.1-dp2'","build:'1.0.0'").replace("'3.0.1-dp2'].includes","'3.0.1-dp2','1.0.0'].includes").replace('smksynth-dp2-sessao.json','smksynth-sessao.json')
s=s.replace("const wasm=await response.arrayBuffer();","const wasm=await response.arrayBuffer();if(!store){const seed=await fetch('factory-session.bin');if(!seed.ok)throw Error('Exemplos de fábrica indisponíveis');store=await seed.arrayBuffer();}")
s='\n'.join(x for x in s.splitlines() if not x.startswith("$('#loadAnalog')") and not x.startswith("$('#loadDrums')"))+'\n'
p.write_text(s)
# Main panel is the established visual. Keep advanced test bindings outside the public UI.
p=sim/'index.html';lines=p.read_text().splitlines();s='\n'.join(lines[:14]);s=s.replace('SMKSYNTH DP2 · SPLIT + LAYER · Drum Pads Lab','SMKSYNTH — Simulador').replace('SMK-37 LAB','SMK-37 PRO').replace('3.0.1 DP2 · ANALOG + DRUM-X · DP2 · SPLIT + LAYER','1.0.0 · ANALOG + DRUM-X').replace('FIRMWARE EM WEBASSEMBLY','SIMULADOR').replace('SMKSYNTH. 16 patterns.','SMKSYNTH. Pronto para tocar.').replace('ANALOG + DRUM-X · Split & Layer · 16 patterns completos.','Cinco patterns de exemplo · oito favoritos · a mesma experiência do instrumento.').replace('ANALOG + DRUM-X · DP2 · SPLIT + LAYER','SMKSYNTH 1.0.0')
s+='''
<details class="lab"><summary>Ajuda e sessão do simulador</summary><div class="labgrid"><section><h2>EXPERIMENTE</h2><p>Inicie o simulador e aperte PLAY para ouvir o primeiro exemplo. KNOB BANK percorre Performance → Split/Layer → Patterns. No banco Patterns, toque em P1–P5 para escolher os exemplos. P6–P16 estão livres.</p><p>No banco Performance, os oito pads inferiores recuperam seus favoritos. Em uma parte DRUM-X, o ciclo de KNOB BANK acrescenta Drum Pads. Escolher bateria não muda os pads automaticamente.</p><p>K1–K4 alteram os controles da tela; K6 escolhe a parte. Para editar a engine, toque GLOBE até PART SETUP e use K1 para escolher ANALOG/DRUM-X. O modo KEY TRANS permanece ativo até ser desligado; K4 na Performance mostra KEY TR quando ligado.</p></section><section><h2>SUA SESSÃO</h2><p>O simulador usa memória própria deste navegador. Seu teclado físico não é alterado. Faça SAVE no painel para gravar timbres/patterns; exporte a sessão para guardá-la.</p><button id="export">Exportar sessão</button><label class="filebtn">Importar sessão<input id="import" type="file" accept=".json"></label><button id="restart">Reiniciar simulador</button><p>Mouse/toque nas teclas e pads; arraste knobs para cima ou para baixo. Teclas A W S E D F T G Y H U J K O L P ; também tocam.</p></section></div></details>
<div hidden><button id="baseline"></button><button id="compare"></button><pre id="comparison"></pre><pre id="writeCounters"></pre><button id="analogA"></button><button id="analogB"></button><select id="drumPart"><option value="0">A</option></select><select id="drumKit"><option value="0">808</option></select><button id="loadDrums"></button></div>
<footer><span><a href="../">SMKSYNTH</a> · <a href="../install/">Instalar no SMK-37 Pro</a></span><span>OberVave / zvenson e colaboradores · DRUM-X / JIANT · GPL-3.0</span></footer></main><script type="module" src="app.js"></script></body></html>
'''
p.write_text(s)
print('Release web assets prepared; firmware SHA256',pins['release']['sha'])
