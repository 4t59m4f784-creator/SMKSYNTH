// SPDX-License-Identifier: GPL-3.0-only
import {Updater} from './protocol.js';
import {productOf,logicalImage,filesOk} from './package.js';
import {PINS} from './pins.js';
import {t} from './language.js';
export {PINS};
export const PENDING='smksynth.1.2.0.pro-elite.pending.v1';
const PRO=["19615", "19616", "19617", "19618", "19619", "19620", "19621", "19622", "19623", "19624", "19625", "19626", "19627", "19628", "19630", "19631", "19632", "19633", "19634", "19635", "19636", "19637", "19638", "19639", "19640", "19641", "19642", "19643", "19644", "19645", "19646", "19647", "19648", "19649"];
const ELITE=["19618", "19629", "19634", "19635", "19636", "19637", "19638", "19639", "19640", "19641", "19642", "19643", "19644", "19645", "19646", "19647", "19648", "19649"];
export function modelOf(id){
 for(const model of Object.keys(PINS)){const name=PINS[model].model;if([name,'ota-'+name].includes(id?.model)&&new RegExp('^(ota-)?'+name+'_[0-9]{3,5}$').test(id?.text||''))return model;}
 throw Error(t('Modelo não compatível. Conecte SMK-37 Pro ou SMK-37 Elite.','Unsupported model. Connect an SMK-37 Pro or SMK-37 Elite.'));
}
const allowed=(model,kind,source)=>{const name=PINS[model]?.model;if(!name)return false;return kind==='release'?source===name+'_016':kind==='update'&&(model==='pro'?PRO:ELITE).some(v=>source===name+'_'+v);};
export async function validate(raw,model,kind){
 const pin=PINS[model]?.[kind];if(!pin||raw.length!==pin.size)throw Error(t('Pacote ou tamanho incorreto.','Wrong package or size.'));
 if(!globalThis.crypto?.subtle)throw Error(t('Use HTTPS e Chrome/Edge no computador.','Use HTTPS and desktop Chrome/Edge.'));
 const sha=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',raw)),v=>v.toString(16).padStart(2,'0')).join('');
 if(sha!==pin.sha)throw Error(t('O pacote não corresponde ao arquivo aprovado.','Package does not match the approved file.'));
 const product=productOf(raw),image=logicalImage(raw);if(product!==pin.product||!filesOk(image))throw Error(t('Modelo ou integridade do pacote inválidos.','Invalid package model or integrity.'));
 return {product,image,sha};
}
export function readPending(storage){try{const p=JSON.parse(storage.getItem(PENDING));return p&&PINS[p.model]?.[p.kind]?.sha===p.sha&&allowed(p.model,p.kind,p.source)?p:null;}catch{return null;}}
export function chooseOperation(id,pending){
 const model=modelOf(id),name=PINS[model].model;
 if(id.text==='ota-'+name+'_900'){
  if(pending?.model===model&&allowed(model,pending.kind,pending.source)&&pending.sha===PINS[model][pending.kind]?.sha)return {model,kind:pending.kind,mode:'resume',source:pending.source,deviceIdentity:id.text};
  throw Error(t('Atualização anterior pendente. Retome pelo instalador que iniciou a transferência.','An earlier update is pending. Resume with the installer that started the transfer.'));
 }
 const kind=id.text===name+'_016'?'release':'update';if(!allowed(model,kind,id.text))throw Error(t('Firmware de origem não habilitado: ','Unsupported source firmware: ')+id.text);
 return {model,kind,mode:'install',source:id.text,deviceIdentity:id.text};
}
export class InstallService{
 constructor(access,storage,log=()=>{},UpdaterType=Updater){Object.assign(this,{access,storage,log,UpdaterType});}
 async identify(){const up=new this.UpdaterType(this.access,this.log);const d=await up.find();if(!d)throw Error(t('Teclado não encontrado. Confira o cabo USB e feche outros aplicativos MIDI.','Keyboard not found. Check the USB cable and close other MIDI apps.'));d.link.close();return d.id;}
 async execute(selection,buffers,step=()=>{}){
  const {model,kind}=selection||{};if(!PINS[model]?.[kind])throw Error(t('Operação inválida.','Invalid operation.'));
  const checked={};for(const k of ['release','update'])checked[k]=await validate(buffers[model]?.[k]||new Uint8Array(),model,k);
  const pkg=checked[kind];let current;
  const guard=(phase,id)=>{if(phase==='start'&&id.text!==current)throw Error(t('A identidade mudou antes da gravação.','Identity changed before writing.'));if(phase!=='start'&&id.text!=='ota-'+PINS[model].model+'_900')throw Error(t('Loader de outro modelo recusado.','A different model loader was rejected.'));};
  const up=new this.UpdaterType(this.access,this.log,{guard});const d=await up.find();if(!d)throw Error(t('Teclado não encontrado.','Keyboard not found.'));const id=d.id;d.link.close();current=id.text;
  const actual=chooseOperation(id,readPending(this.storage));if(actual.model!==model||actual.kind!==kind||actual.deviceIdentity!==selection.deviceIdentity||actual.source!==selection.source)throw Error(t('O teclado mudou. Conecte e confira novamente.','The keyboard changed. Connect and check again.'));
  this.storage.setItem(PENDING,JSON.stringify({model,kind,sha:pkg.sha,source:actual.source}));
  if(actual.mode==='resume'){if(!await up.resume(pkg.image,step,{product:pkg.product}))throw Error(t('Retomada incompleta. Guarde o log.','Resume incomplete. Keep the log.'));}else await up.install(pkg.image,pkg.product,step);
  this.storage.removeItem(PENDING);return pkg.product;
 }
}
