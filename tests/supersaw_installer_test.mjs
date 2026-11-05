// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {webcrypto} from 'node:crypto';
if(!globalThis.crypto)Object.defineProperty(globalThis,'crypto',{value:webcrypto});
import {PINS,PENDING,validate,modeFor,InstallService} from '../experimental/supersaw/service.js';
const buffers={};for(const kind of Object.keys(PINS)){
 buffers[kind]=new Uint8Array(fs.readFileSync(new URL('../experimental/supersaw/'+PINS[kind].url,import.meta.url)));
 await validate(buffers[kind],kind);const bad=buffers[kind].slice();bad[900]^=1;await assert.rejects(()=>validate(bad,kind));
}
const id=text=>({text,model:text.split('_')[0]});
assert.equal(modeFor('release',id('SMK-37 Pro_19619')),'install');
for(const text of ['SMK-37 Pro_19618','SMK-37 Pro_19619','SMK-37 Pro_19620','SMK-37 Pro_19621']){assert.equal(modeFor('update',id(text)),'install');if(text.endsWith('19618'))assert.throws(()=>modeFor('release',id(text)));else assert.equal(modeFor('release',id(text)),'install');}
assert.throws(()=>modeFor('update',id('SMK-37 Pro_016')));
for(const kind of Object.keys(PINS)){
 for(const text of ['SMK-37 Pro_016','SMK-37 Elite_19618','FM-1_016','SMK-37 Pro_015','MKE-P37_19618'])assert.throws(()=>modeFor(kind,id(text)));
 const source=kind==='release'?'SMK-37 Pro_19619':'SMK-37 Pro_19618';
 const pending={kind,sha:PINS[kind].sha,source};assert.equal(modeFor(kind,id('ota-SMK-37 Pro_900'),pending),'resume');
 assert.throws(()=>modeFor(kind,id('ota-SMK-37 Pro_900'),{...pending,sha:'bad'}));
}
let source='SMK-37 Pro_19618',fail=false,queries=0;
class U{constructor(a,l,o){this.o=o;}async find(){queries++;return{id:id(source),link:{close(){}}};}async install(image,product){this.o.guard('start',id(source));this.o.guard('write',id('ota-SMK-37 Pro_900'));if(fail)throw Error('cut');source=product;}async resume(image,step,{product}){this.o.guard('write',id(source));if(fail)throw Error('cut again');source=product;return true;}}
const map=new Map(),storage={getItem:k=>map.get(k),setItem:(k,v)=>map.set(k,v),removeItem:k=>map.delete(k)},service=new InstallService({},storage,()=>{},U);
await assert.rejects(()=>service.execute('update',{...buffers,release:buffers.release.slice(1)}));assert.equal(queries,0);
fail=true;await assert.rejects(()=>service.execute('update',buffers));source='ota-SMK-37 Pro_900';await assert.rejects(()=>service.execute('update',buffers));assert.equal(JSON.parse(map.get(PENDING)).source,'SMK-37 Pro_19618');fail=false;await service.execute('update',buffers);assert(!map.has(PENDING));
source='SMK-37 Pro_19619';await service.execute('release',buffers);assert.equal(source,'SMK-37 Pro_19618');
source='SMK-37 Pro_19620';await service.execute('release',buffers);assert.equal(source,'SMK-37 Pro_19618');
console.log('PASS: pinned SS3/stable-return, SS1/SS2 upgrades and SS3 reinstall, strict per-operation origins, same-ID update, two interrupted resumes retain original source, wrong model/source and tamper refusal before writes.');

const legacy=await import('../experimental/supersaw/legacy-ss1/service.js');
for(const k of Object.keys(legacy.PINS)){
 const raw=new Uint8Array(fs.readFileSync(new URL('../experimental/supersaw/legacy-ss1/'+legacy.PINS[k].url,import.meta.url)));
 await legacy.validate(raw,k);
}
assert.notEqual(legacy.PENDING,PENDING);
assert.equal(legacy.modeFor('update',id('ota-SMK-37 Pro_900'),{kind:'update',sha:legacy.PINS.update.sha,source:'SMK-37 Pro_19618'}),'resume');
console.log('PASS: frozen SS1 packages/keys and interrupted-install resume preserved.');

const ss2=await import('../experimental/supersaw/legacy-ss2/service.js');
for(const k of Object.keys(ss2.PINS)){
 const raw=new Uint8Array(fs.readFileSync(new URL('../experimental/supersaw/legacy-ss2/'+ss2.PINS[k].url,import.meta.url)));
 await ss2.validate(raw,k);
}
assert.notEqual(ss2.PENDING,PENDING);
assert.equal(ss2.modeFor('update',id('ota-SMK-37 Pro_900'),{kind:'update',sha:ss2.PINS.update.sha,source:'SMK-37 Pro_19619'}),'resume');
source='SMK-37 Pro_19621';await service.execute('release',buffers);assert.equal(source,'SMK-37 Pro_19618');
console.log('PASS: frozen SS2 resume keys/packages, SS3->stable return.');
