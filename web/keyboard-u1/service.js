// SPDX-License-Identifier: GPL-3.0-only
import {Updater} from './protocol.js';
import {productOf,logicalImage,filesOk} from './package.js';
export const PINS=Object.freeze({"u1": {"name": "SMKSYNTH U1 · Pad banks e navegação", "product": "SMK-37 Pro_19609", "size": 651327, "sha": "48e8d90c8bc1c9bd451d5f213696c6de44e804e149cd0294b9edd2955485619f", "url": "./firmware/obervave-3.0.1-u1-smk37.fwsc"}, "br1": {"name": "SMKSYNTH BR1 · Identidade visual", "product": "SMK-37 Pro_19608", "size": 651327, "sha": "d212761d8524b4404e978b91cd981d77b6983344b2d72116b08ae1bc7817e4e8", "url": "./firmware/obervave-3.0.1-br1-smk37.fwsc"}, "sl2": {"name": "SMKSYNTH SL2 · Duas linhas de Split & Layer", "product": "SMK-37 Pro_19607", "size": 651327, "sha": "d732c034724cbd8618797441f021264ed185ce35130e0394e362a5cdaf7cc6e7", "url": "./firmware/obervave-3.0.1-sl2-smk37.fwsc"}, "sl1": {"name": "SMKSYNTH SL1 · Split & Layer experimental", "product": "SMK-37 Pro_19606", "size": 651327, "sha": "a5aaa1bddb4a413199151599b2f72e1b6f9bcd84859f1e462475a76d3b3a38cc", "url": "./firmware/obervave-3.0.1-sl1-smk37.fwsc"}, "ad1": {"name": "SMKSYNTH AD1 · ANALOG + DRUM-X + SEQ TRANSPOSE", "product": "SMK-37 Pro_19605", "size": 651327, "sha": "7d91de2be983bb3805e350e34f0bfbd78278fa01e833ffa499cceb86cefe784a", "url": "./firmware/obervave-3.0.1-ad1-smk37.fwsc"}, "pf12": {"name": "Player PF12 · base preservada", "product": "SMK-37 Pro_19552", "size": 651327, "sha": "934a048261d349d1995a980e03e48290937fd2d7b104389713fa359230a190cd", "url": "./firmware/obervave-3.0.1-pf12-smk37.fwsc"}, "pf9": {"name": "Player PF9 · referência aprovada no teclado", "product": "SMK-37 Pro_19549", "size": 651327, "sha": "2b9406fbecc61960e51d5da173c072107a81bb75c79fb5f35d06dd27f41f88f6", "url": "./firmware/obervave-3.0.1-pf9-smk37.fwsc"}});
export const PENDING="smksynth.u1.pending.v1";
export async function validate(raw,kind){
 const p=PINS[kind];if(!p)throw Error('Operação inválida.');
 if(raw.length!==p.size)throw Error('Tamanho diferente do pacote esperado.');
 if(!crypto.subtle)throw Error('Abra este instalador em HTTPS para verificar o arquivo.');
 const sha=[...new Uint8Array(await crypto.subtle.digest('SHA-256',raw))].map(x=>x.toString(16).padStart(2,'0')).join('');
 if(sha!==p.sha)throw Error('SHA-256 diferente. Selecione o arquivo original, sem alterações.');
 const product=productOf(raw),image=logicalImage(raw);
 if(product!==p.product||!filesOk(image))throw Error('Identidade ou CRCs inválidos.');
 return {raw,image,product,sha};
}
export function readPending(storage){try{const p=JSON.parse(storage.getItem(PENDING));return p&&PINS[p.kind]?.sha===p.sha?p:null}catch{return null}}
const SOURCES=new Set(['016','451','452','453','454','9530','19531','19541','19542','19543','19545','19546','19547','19548','19549','19550','19551','19552','19601','19602','19603','19604','19605','19606','19607','19608','19609','000'].map(v=>'SMK-37 Pro_'+v));
export function modeFor(kind,id,pending){
 if(!PINS[kind])throw Error('Operação inválida.');
 if(!id?.text)throw Error('Conecte e consulte o teclado.');
 if(!['SMK-37 Pro','ota-SMK-37 Pro'].includes(id.model))throw Error('Modelo recusado: somente SMK-37 Pro.');
 if(SOURCES.has(id.text))return 'install';
 if(id.text==='ota-SMK-37 Pro_900'&&(kind!=='u1'||pending?.kind===kind&&pending.sha===PINS[kind].sha))return 'resume';
 throw Error('Identidade não habilitada: '+id.text+'. Nenhuma gravação iniciada. Baixe o log.');
}
function guardFor(source){return (phase,id)=>{
 if(phase==='start'&&id.text!==source())throw Error('A identidade mudou antes da gravação: '+id.text);
 if(phase!=='start'&&id.text!=='ota-SMK-37 Pro_900')throw Error('Loader recusado: '+id.text);
}}
export class InstallService{
 constructor(access,storage,log=()=>{},UpdaterType=Updater){this.access=access;this.storage=storage;this.log=log;this.UpdaterType=UpdaterType}
 async identify(){const up=new this.UpdaterType(this.access,this.log);const d=await up.find();if(!d)throw Error('Teclado não encontrado. Confira o cabo USB de dados e feche outros aplicativos MIDI.');d.link.close();return d.id}
 async execute(kind,buffers,step=()=>{}){
  // Every install/return package must be present and pinned before a MIDI query.
  const validated={};for(const k of Object.keys(PINS))validated[k]=await validate(buffers[k],k);
  if(!PINS[kind])throw Error('Operação inválida.');const pkg=validated[kind];let source;
  const up=new this.UpdaterType(this.access,this.log,{guard:guardFor(()=>source)});
  const d=await up.find();if(!d)throw Error('Teclado não encontrado.');
  const id=d.id;source=id.text;d.link.close();const mode=modeFor(kind,id,readPending(this.storage));
  this.storage.setItem(PENDING,JSON.stringify({kind,sha:PINS[kind].sha,source}));
  if(mode==='resume'){if(!await up.resume(pkg.image,step,{product:pkg.product}))throw Error('Retomada incompleta. Preserve o log.');}
  else await up.install(pkg.image,pkg.product,step);
  this.storage.removeItem(PENDING);return pkg.product;
 }
}
