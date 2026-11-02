// SPDX-License-Identifier: GPL-3.0-only
import {Updater} from './protocol.js';
import {productOf,logicalImage,filesOk} from './package.js';
export const PINS=Object.freeze({"b2": {"name": "SMKSYNTH B2 · exportação e restauração", "product": "SMK-37 Pro_19615", "size": 651819, "sha": "50054674cc3db57c971321b9f94993099f1a5505125917dd23400588914a5685", "url": "./firmware/obervave-3.0.1-b2-smk37.fwsc"}});
export const PENDING="smksynth.b2.pending.v1";
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
const SOURCES=new Set(['SMK-37 Pro_19613','SMK-37 Pro_19614','SMK-37 Pro_19615']);
export function modeFor(kind,id,pending){
 if(!PINS[kind])throw Error('Operação inválida.');
 if(!id?.text)throw Error('Conecte e consulte o teclado.');
 if(!['SMK-37 Pro','ota-SMK-37 Pro'].includes(id.model))throw Error('Modelo recusado: somente SMK-37 Pro.');
 if(SOURCES.has(id.text))return 'install';
 if(id.text==='ota-SMK-37 Pro_900'&&(pending?.kind===kind&&pending.sha===PINS[kind].sha))return 'resume';
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
