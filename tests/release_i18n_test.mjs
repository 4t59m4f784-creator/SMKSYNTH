// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';
import vm from 'node:vm';
import assert from 'node:assert/strict';
const source=fs.readFileSync('web/site/i18n.js','utf8');
function load(lang){const values=new Map();const document={documentElement:{},readyState:'loading',addEventListener(){}};const window={confirm:s=>s};vm.runInNewContext(source,{URLSearchParams,URL,location:{search:lang?'?lang='+lang:'',href:'https://example.test/SMKSYNTH/'},localStorage:{getItem:k=>values.get(k),setItem:(k,v)=>values.set(k,v)},document,window,WeakMap});return {translate:window.smksynthText,document};}
const en=load('en'),pt=load('pt');
assert.equal(en.document.documentElement.lang,'en');assert.equal(pt.document.documentElement.lang,'pt-BR');
for(const [input,want] of [
 ['Conectar e consultar','Connect and identify'],
 ['Selecione um arquivo SMKSYNTH 1.1.0 ou de uma versão anterior compatível.','Select a SMKSYNTH 1.1.0 file or a compatible earlier version.'],
 ['SMKSYNTH 1.1.0 executando · áudio ativo · memória independente do teclado','SMKSYNTH 1.1.0 running · audio active · memory independent of the keyboard'],
 ['Identidade informada: SMK-37 Pro_016','Reported identity: SMK-37 Pro_016'],
 ['Exportação não concluída: Sessão indisponível ou expirada.','Export did not complete: Session unavailable or expired.'],
 ['PADRÃO 5 · 16 PADS · P1–P16','PATTERN 5 · 16 PADS · P1–P16'],
 ['9–16 favoritos: toque para carregar, segure 0,7 s para memorizar.','9–16 favourites: tap to load, hold 0.7 s to store.'],
 ['Digite RESTAURAR para habilitar a confirmação','Type RESTAURAR to enable confirmation'],
 ['Substituir a memória local do simulador pela sessão importada?','Replace the simulator’s local memory with the imported session?']
]){assert.equal(en.translate(input),want);assert.equal(pt.translate(input),input);}
for(const s of ['SMK-37 Pro_016','SMK-37 Pro_19618','ota-SMK-37 Pro_900','SMK16P1','RESTAURAR','ANALOG','DRUM-X','SEQ','PLAY','REC'])assert.equal(en.translate(s),s);
for(const name of ['site','install','backup','simulator']){
 const html=fs.readFileSync(`web/${name}/index.html`,'utf8');assert.ok(html.includes('i18n.js'));assert.ok(!html.includes('noindex,nofollow'));
}
for(const doc of ['INSTALLATION','USER_GUIDE','BUILD','VALIDATION'])assert.ok(fs.statSync(`docs/${doc}.en.md`).size>1000);
console.log('PASS: PT/EN messages, safety confirmations, protocol identifiers, language hooks and English guides');
