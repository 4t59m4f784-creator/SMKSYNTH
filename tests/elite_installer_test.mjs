// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {webcrypto} from 'node:crypto';
import {PINS,PENDING,validate,modeFor,InstallService} from '../web/install-elite/service.js';
if(!globalThis.crypto)Object.defineProperty(globalThis,'crypto',{value:webcrypto});
assert.deepEqual(PINS,JSON.parse(fs.readFileSync(new URL('../web/install-elite/manifest.json',import.meta.url),'utf8')));
const buffers={};for(const kind of ['release','update'])buffers[kind]=new Uint8Array(fs.readFileSync(new URL('../web/install-elite/'+PINS[kind].url,import.meta.url)));
if(process.argv[2])buffers.stock=new Uint8Array(fs.readFileSync(process.argv[2]));
const id=text=>({text,model:text.split('_')[0]});
assert.equal(modeFor('release',id('SMK-37 Elite_016')),'install');
assert.equal(modeFor('update',id('SMK-37 Elite_19629')),'install');
assert.equal(modeFor('stock',id('SMK-37 Elite_19629')),'install');
assert.equal(modeFor('stock',id('SMK-37 Elite_000')),'install');
assert.throws(()=>modeFor('release',id('SMK-37 Elite_000')));
assert.throws(()=>modeFor('update',id('SMK-37 Elite_016')));
for(const kind of Object.keys(PINS)){
 for(const text of ['SMK-37 Pro_016','SMK-37 Pro_19618','FM-1_016','SMK-37 Elite_015','SMK-37 Elite_99999','MKE-P37_016'])assert.throws(()=>modeFor(kind,id(text)));
 const source=kind==='release'?'SMK-37 Elite_016':'SMK-37 Elite_19629',pending={kind,sha:PINS[kind].sha,source};
 assert.equal(modeFor(kind,id('ota-SMK-37 Elite_900'),pending),'resume');
 assert.throws(()=>modeFor(kind,id('ota-SMK-37 Elite_900'),{...pending,sha:'bad'}));
 assert.throws(()=>modeFor(kind,id('ota-SMK-37 Pro_900'),pending));
 if(kind==='stock')assert.equal(modeFor(kind,id('ota-SMK-37 Elite_016'),pending),'resume');
 else assert.throws(()=>modeFor(kind,id('ota-SMK-37 Elite_016'),pending));
}
let source,loader,cut,queries=0,writes=0,changed=false;
class Updater{
 constructor(a,l,options){this.options=options;}
 async find(){queries++;return {id:id(source),link:{close(){}}};}
 async install(image,product){this.options.guard('start',id(changed?'SMK-37 Pro_19618':source));this.options.guard('write',id(loader));writes++;if(cut)throw Error('cut');source=product;}
 async resume(image,step,{product}){this.options.guard('write',id(source));writes++;if(cut)throw Error('cut');source=product;return true;}
}
const map=new Map(),storage={getItem:k=>map.get(k),setItem:(k,v)=>map.set(k,v),removeItem:k=>map.delete(k)},service=new InstallService({},storage,()=>{},Updater);
for(const kind of Object.keys(buffers)){
 await validate(buffers[kind],kind);
 const bad=buffers[kind].slice();bad[900]^=1;await assert.rejects(()=>validate(bad,kind));
 const before=queries;await assert.rejects(()=>service.execute(kind,{...buffers,[kind]:bad}));assert.equal(queries,before,'Tamper must fail before MIDI');
 source=kind==='release'?'SMK-37 Elite_016':'SMK-37 Elite_19629';loader=kind==='stock'?'ota-SMK-37 Elite_016':'ota-SMK-37 Elite_900';
 const original=source;cut=true;await assert.rejects(()=>service.execute(kind,buffers));source=loader;await assert.rejects(()=>service.execute(kind,buffers));assert.equal(JSON.parse(map.get(PENDING)).source,original);
 cut=false;await service.execute(kind,buffers);assert.equal(source,PINS[kind].product);assert(!map.has(PENDING));
}
source='SMK-37 Pro_016';const before=writes;await assert.rejects(()=>service.execute('release',buffers));assert.equal(writes,before);
source='SMK-37 Elite_016';loader='ota-SMK-37 Pro_900';await assert.rejects(()=>service.execute('release',buffers));assert.equal(writes,before);
map.clear();source='SMK-37 Elite_016';loader='ota-SMK-37 Elite_900';changed=true;await assert.rejects(()=>service.execute('release',buffers));assert.equal(writes,before);
console.log('PASS: Elite-only stock V016 first install, preserving update, stock return/recovery, exact pins, tamper before MIDI, loader/model refusal, bound interrupted resumes');
