// SPDX-License-Identifier: GPL-3.0-only
import {Updater} from './protocol.js';
import {productOf,logicalImage,filesOk} from './package.js';
export const PINS=Object.freeze({"pro.update": {"name": "Instalar PS2 · Endless MIDI · Pro", "product": "SMK-37 Pro_19666", "url": "./SMKSYNTH-1.3.0-PS2-Pro.fwsc", "size": 651876, "sha": "ec7f63be2cd68daf9ade0a56b3b28fb7d5b58998d2ba880c09eb7e70903beed3"}, "pro.release": {"name": "Retornar à oficial 1.3.0 · Pro", "product": "SMK-37 Pro_19664", "url": "./SMKSYNTH-1.3.0-oficial-Pro.fwsc", "size": 651876, "sha": "ed789dcf52e6fa7999ac6a9b121dba0c1d9e564d5fe59575cc30436dc72983b7"}, "elite.update": {"name": "Instalar PS2 · Endless MIDI · Elite", "product": "SMK-37 Elite_19666", "url": "./SMKSYNTH-1.3.0-PS2-Elite.fwsc", "size": 651880, "sha": "fd4146cb79590b497831c69e4c389178f6728d0e54bdc6febafd0ef934267d94"}, "elite.release": {"name": "Retornar à oficial 1.3.0 · Elite", "product": "SMK-37 Elite_19664", "url": "./SMKSYNTH-1.3.0-oficial-Elite.fwsc", "size": 651880, "sha": "c5393139a45d7c59f9751b9cc5e7b5d6659a8d9af82dc64e51d464bdcfbe2173"}});
export const PENDING='smksynth.endless.ps2.pending.v1';
export function modelKey(id){const m=id?.model?.replace(/^ota-/,'');if(m==='SMK-37 Pro')return 'pro';if(m==='SMK-37 Elite')return 'elite';throw Error('Modelo recusado. Esta experimental aceita somente SMK-37 Pro ou Elite.');}
export const pinKey=(kind,id)=>modelKey(id)+'.'+kind;
export async function validate(raw,kind){
 const p=PINS[kind];if(!p)throw Error('Operação inválida.');
 if(raw.length!==p.size)throw Error('Tamanho diferente do pacote esperado.');
 if(!crypto.subtle)throw Error('Abra este instalador em localhost ou HTTPS para verificar o arquivo.');
 const sha=[...new Uint8Array(await crypto.subtle.digest('SHA-256',raw))].map(x=>x.toString(16).padStart(2,'0')).join('');
 if(sha!==p.sha)throw Error('SHA-256 diferente. Selecione o arquivo original, sem alterações.');
 const product=productOf(raw),image=logicalImage(raw);
 if(product!==p.product||!filesOk(image))throw Error('Identidade ou CRCs inválidos.');
 return {raw,image,product,sha};
}

const sourceAllowed=(kind,text)=>{const m=/^(SMK-37 Pro|SMK-37 Elite)_(\d+)$/.exec(text||'');return !!m&&(m[1]==='SMK-37 Pro'?["19615", "19616", "19617", "19618", "19619", "19620", "19621", "19622", "19623", "19624", "19625", "19626", "19627", "19628", "19630", "19631", "19632", "19633", "19634", "19635", "19636", "19637", "19638", "19639", "19640", "19641", "19642", "19643", "19644", "19645", "19646", "19647", "19648", "19649", "19650", "19651", "19652", "19653", "19654", "19655", "19656", "19657", "19658", "19659", "19660", "19661", "19662", "19663", "19664", "19665", "19666"]:["19618", "19629", "19634", "19635", "19636", "19637", "19638", "19639", "19640", "19641", "19642", "19643", "19644", "19645", "19646", "19647", "19648", "19649", "19650", "19651", "19652", "19653", "19654", "19655", "19656", "19657", "19658", "19659", "19660", "19661", "19662", "19663", "19664", "19665", "19666"]).includes(m[2]);};
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
