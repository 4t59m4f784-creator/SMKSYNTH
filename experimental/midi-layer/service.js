// SPDX-License-Identifier: GPL-3.0-only
import {Updater} from './protocol.js';
import {productOf,logicalImage,filesOk} from './package.js';
export const PINS=Object.freeze({"pro.update": {"name": "Instalar ML1 · Pro", "product": "SMK-37 Pro_19650", "url": "./SMKSYNTH-1.2.0-ml1-SMK37-Pro.fwsc", "size": 651819, "sha": "e909d425652f5abd08172ac27a17ed7d27f14ad5913f40ca230405660df735dc"}, "pro.release": {"name": "Retornar à oficial 1.2.0 · Pro", "product": "SMK-37 Pro_19649", "url": "./SMKSYNTH-1.2.0-return-SMK37-Pro.fwsc", "size": 651819, "sha": "ae19658ed7160a12046db99fc2d3570aa10b653f7d1d9f17a3fa6dbf7ff1295c"}, "elite.update": {"name": "Instalar ML1 · Elite", "product": "SMK-37 Elite_19650", "url": "./SMKSYNTH-1.2.0-ml1-SMK37-Elite.fwsc", "size": 651819, "sha": "20442418d7647c55c43ba83c108d584cd6597abd227e6e346e53d1d586e0c952"}, "elite.release": {"name": "Retornar à oficial 1.2.0 · Elite", "product": "SMK-37 Elite_19649", "url": "./SMKSYNTH-1.2.0-return-SMK37-Elite.fwsc", "size": 651819, "sha": "f7b87830f61b2b23b62d497bcbebc8ed5519a16a3396e3c7ae595d164ece1fde"}});
export const PENDING='smksynth.midi.ml1.pending.v1';
export function modelKey(id){const m=id?.model?.replace(/^ota-/,'');if(m==='SMK-37 Pro')return 'pro';if(m==='SMK-37 Elite')return 'elite';throw Error('Modelo recusado. Esta experimental aceita somente SMK-37 Pro ou Elite.');}
export const pinKey=(kind,id)=>modelKey(id)+'.'+kind;
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

const sourceAllowed=(kind,text)=>{const m=/^(SMK-37 Pro|SMK-37 Elite)_(\d+)$/.exec(text||'');if(!m)return false;const v=Number(m[2]);return m[1]==='SMK-37 Pro'?v>=19618&&v<=19650&&v!==19629:[19618,19629,19634,...Array.from({length:16},(_,i)=>19635+i)].includes(v);};
export function readPending(storage,id){try{const p=JSON.parse(storage.getItem(PENDING+'.'+modelKey(id)));return p&&p.model===modelKey(id)&&PINS[p.model+'.'+p.kind]?.sha===p.sha?p:null}catch{return null;}}
export function modeFor(kind,id,pending){
 if(!['update','release'].includes(kind))throw Error('Operação inválida.');const model=modelKey(id),pin=PINS[model+'.'+kind],name=model==='pro'?'SMK-37 Pro':'SMK-37 Elite';
 if(id.text.startsWith(name+'_')&&sourceAllowed(kind,id.text))return 'install';
 if(id.text==='ota-'+name+'_900'&&pending?.model===model&&pending.kind===kind&&pending.sha===pin.sha&&sourceAllowed(kind,pending.source)&&pending.source.startsWith(name+'_'))return 'resume';
 throw Error('Identidade não habilitada: '+id.text+'. Atualize a partir de SMKSYNTH; a instalação inicial pelo firmware M-VAVE não é feita nesta página.');
}
export class InstallService{
 constructor(access,storage,log=()=>{},UpdaterType=Updater){Object.assign(this,{access,storage,log,UpdaterType});}
 async identify(){const up=new this.UpdaterType(this.access,this.log),d=await up.find();if(!d)throw Error('Teclado não encontrado. Confira cabo USB e feche os outros aplicativos MIDI.');d.link.close();modelKey(d.id);return d.id;}
 async execute(kind,buffers,step=()=>{},expectedIdentity=null){
  const validated={};for(const key of Object.keys(PINS))validated[key]=await validate(buffers[key],key);
  let source,model;const up=new this.UpdaterType(this.access,this.log,{guard:(phase,id)=>{if(phase==='start'){if(id.text!==source)throw Error('A identidade mudou antes da gravação.');}else{const expected='ota-'+(model==='pro'?'SMK-37 Pro':'SMK-37 Elite')+'_900';if(id.text!==expected)throw Error('Loader/modelo recusado: '+id.text);}}});
  const d=await up.find();if(!d)throw Error('Teclado não encontrado.');const id=d.id;d.link.close();if(expectedIdentity&&id.text!==expectedIdentity)throw Error("O teclado mudou após a confirmação. Consulte novamente.");model=modelKey(id);const pending=readPending(this.storage,id),mode=modeFor(kind,id,pending),pkg=validated[model+'.'+kind];source=mode==='resume'?pending.source:id.text;
  const key=PENDING+'.'+model;this.storage.setItem(key,JSON.stringify({model,kind,sha:pkg.sha,source}));
  if(mode==='resume'){if(!await up.resume(pkg.image,step,{product:pkg.product}))throw Error('Retomada incompleta. Preserve o log.');}else await up.install(pkg.image,pkg.product,step);
  this.storage.removeItem(key);return pkg.product;
 }
}
