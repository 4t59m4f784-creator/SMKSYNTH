// SPDX-License-Identifier: GPL-3.0-only
import {Updater} from './protocol.js';
import {productOf,logicalImage,filesOk} from './package.js';
export const PINS=Object.freeze({"update": {"name": "Instalar ENDLESS E2", "product": "SMK-37 Pro_19624", "url": "./SMKSYNTH-1.0.3-e2-SMK37-Pro.fwsc", "size": 651819, "sha": "d95ca89958b079e97baf651c987f170ff9d36d6526982b4b611af101314e605c"}, "release": {"name": "Retornar à pública 1.0.3", "product": "SMK-37 Pro_19618", "url": "./SMKSYNTH-1.0.3-return-SMK37-Pro.fwsc", "size": 651819, "sha": "9b2b779f7eaa9d5228fc93c314ae1aa4b9e3ff12ef0c0a11534123da2468e870"}});
export const PENDING="smksynth.endless.e2.pending.v1";
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
const originAllowed=(kind,source)=>kind==='release'?['SMK-37 Pro_19619','SMK-37 Pro_19620','SMK-37 Pro_19621','SMK-37 Pro_19622','SMK-37 Pro_19623','SMK-37 Pro_19624','SMK-37 Pro_19625','SMK-37 Pro_19626','SMK-37 Pro_19627','SMK-37 Pro_19628'].includes(source):kind==='update'&&['SMK-37 Pro_19618','SMK-37 Pro_19619','SMK-37 Pro_19620','SMK-37 Pro_19621','SMK-37 Pro_19622','SMK-37 Pro_19623','SMK-37 Pro_19624','SMK-37 Pro_19625','SMK-37 Pro_19626','SMK-37 Pro_19627','SMK-37 Pro_19628'].includes(source);
export function modeFor(kind,id,pending){
 if(!PINS[kind])throw Error('Operação inválida.');
 if(!id?.text)throw Error('Conecte e consulte o teclado.');
 if(!['SMK-37 Pro','ota-SMK-37 Pro'].includes(id.model))throw Error('Modelo recusado: somente SMK-37 Pro.');
 if(originAllowed(kind,id.text))return 'install';
 if(id.text==='ota-SMK-37 Pro_900'&&(pending?.kind===kind&&pending.sha===PINS[kind].sha&&originAllowed(kind,pending.source)))return 'resume';
 throw Error('Esta página aceita SMKSYNTH 1.0.3, SS1/SS2/SS3, UI1, E1 ou E2 no SMK-37 Pro. Detectado: '+id.text+'. Nenhuma gravação iniciada.');
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
  const id=d.id;const pending=readPending(this.storage);d.link.close();const mode=modeFor(kind,id,pending);source=mode==='resume'?pending.source:id.text;
  this.storage.setItem(PENDING,JSON.stringify({kind,sha:PINS[kind].sha,source}));
  if(mode==='resume'){if(!await up.resume(pkg.image,step,{product:pkg.product}))throw Error('Retomada incompleta. Preserve o log.');}
  else await up.install(pkg.image,pkg.product,step);
  this.storage.removeItem(PENDING);return pkg.product;
 }
}

