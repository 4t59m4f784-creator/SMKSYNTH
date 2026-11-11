// SPDX-License-Identifier: GPL-3.0-only
import {Updater} from '../install/protocol.js';
import {productOf,logicalImage,filesOk} from '../install/package.js';
import {PINS} from './pins.js?v=elite2';
import {t} from './language.js?v=elite2';
export {PINS};
export const PENDING='smksynth.elite2.pending.v1';
const originAllowed=(kind,source)=>kind==='release'?source==='SMK-37 Elite_016':kind==='update'?['SMK-37 Elite_19618','SMK-37 Elite_19629'].includes(source):kind==='stock'&&['SMK-37 Elite_19618','SMK-37 Elite_19629','SMK-37 Elite_000'].includes(source);
export async function validate(raw,kind){
 const pin=PINS[kind];if(!pin||raw.length!==pin.size)throw Error(t('Pacote ou tamanho incorreto.','Wrong package or size.'));
 if(!globalThis.crypto?.subtle)throw Error(t('Use HTTPS e Chrome/Edge no computador.','Use HTTPS and desktop Chrome/Edge.'));
 const sha=[...new Uint8Array(await crypto.subtle.digest('SHA-256',raw))].map(v=>v.toString(16).padStart(2,'0')).join('');
 if(sha!==pin.sha)throw Error(t('SHA-256 diferente do pacote aprovado.','SHA-256 does not match the approved package.'));
 const product=productOf(raw),image=logicalImage(raw);
 if(product!==pin.product||!filesOk(image))throw Error(t('Modelo ou CRC inválido.','Invalid model or CRC.'));
 return {product,image,sha};
}
export function readPending(storage){try{const p=JSON.parse(storage.getItem(PENDING));return p&&PINS[p.kind]?.sha===p.sha&&originAllowed(p.kind,p.source)?p:null}catch{return null}}
export function modeFor(kind,id,pending){
 if(!PINS[kind]||!id?.text)throw Error(t('Conecte e consulte o teclado.','Connect and identify the keyboard.'));
 if(!['SMK-37 Elite','ota-SMK-37 Elite'].includes(id.model))throw Error(t('Esta página aceita somente SMK-37 Elite.','This page only accepts SMK-37 Elite.'));
 if(originAllowed(kind,id.text))return 'install';
 const loaders=kind==='stock'?['ota-SMK-37 Elite_900','ota-SMK-37 Elite_016']:['ota-SMK-37 Elite_900'];
 if(loaders.includes(id.text)&&pending?.kind===kind&&pending.sha===PINS[kind].sha&&originAllowed(kind,pending.source))return 'resume';
 throw Error(t('Origem não habilitada para esta operação: ','Source not enabled for this operation: ')+id.text);
}
export class InstallService{
 constructor(access,storage,log=()=>{},UpdaterType=Updater){this.access=access;this.storage=storage;this.log=log;this.UpdaterType=UpdaterType;}
 async identify(){const up=new this.UpdaterType(this.access,this.log);const found=await up.find();if(!found)throw Error(t('Teclado não encontrado. Confira USB e feche outros programas MIDI.','Keyboard not found. Check USB and close other MIDI applications.'));found.link.close();return found.id;}
 async execute(kind,buffers,step=()=>{}){
  const pkg=await validate(buffers[kind]||new Uint8Array(),kind);
  let current;
  const guard=(phase,id)=>{
   if(phase==='start'&&id.text!==current)throw Error(t('A identidade mudou antes da gravação.','Identity changed before writing.'));
   const loaders=kind==='stock'?['ota-SMK-37 Elite_900','ota-SMK-37 Elite_016']:['ota-SMK-37 Elite_900'];
   if(phase!=='start'&&!loaders.includes(id.text))throw Error(t('Loader recusado: ','Loader rejected: ')+id.text);
  };
  const up=new this.UpdaterType(this.access,this.log,{guard});
  const found=await up.find();if(!found)throw Error(t('Teclado não encontrado.','Keyboard not found.'));
  const id=found.id;found.link.close();current=id.text;
  const pending=readPending(this.storage),mode=modeFor(kind,id,pending),source=mode==='resume'?pending.source:current;
  this.storage.setItem(PENDING,JSON.stringify({kind,sha:pkg.sha,source}));
  if(mode==='resume'){if(!await up.resume(pkg.image,step,{product:pkg.product}))throw Error(t('Retomada incompleta. Guarde o log.','Resume incomplete. Keep the log.'));}
  else await up.install(pkg.image,pkg.product,step);
  this.storage.removeItem(PENDING);return pkg.product;
 }
}
