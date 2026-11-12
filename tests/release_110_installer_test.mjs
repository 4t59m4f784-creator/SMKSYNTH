// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {webcrypto} from 'node:crypto';
import {PINS,PENDING,validate,modelOf,chooseOperation,readPending,InstallService} from '../web/install/service.js';
if(!globalThis.crypto)globalThis.crypto=webcrypto;
assert.deepEqual(PINS,JSON.parse(fs.readFileSync('web/install/manifest.json')));
const id=text=>({model:text.split('_')[0],text});const buffers={};
for(const model of ['pro','elite']){buffers[model]={};for(const kind of ['release','update']){const pin=PINS[model][kind],raw=new Uint8Array(fs.readFileSync('web/install/'+pin.url));buffers[model][kind]=raw;assert.equal((await validate(raw,model,kind)).product,pin.product);const bad=raw.slice();bad[900]^=1;await assert.rejects(()=>validate(bad,model,kind));await assert.rejects(()=>validate(raw,model==='pro'?'elite':'pro',kind));}}
for(const model of ['pro','elite']){
 const name=PINS[model].model;assert.equal(modelOf(id(name+'_016')),model);assert.equal(chooseOperation(id(name+'_016')).kind,'release');
 const origins=model==='pro'?['19615','19616','19617','19618','19619','19620','19621','19622','19623','19624','19625','19626','19627','19628','19630','19631','19632','19633','19634']:['19618','19629','19634'];
 for(const n of origins)assert.equal(chooseOperation(id(name+'_'+n)).kind,'update');
 for(const n of ['015','99999'])assert.throws(()=>chooseOperation(id(name+'_'+n)));
 for(const kind of ['release','update']){const source=name+(kind==='release'?'_016':model==='pro'?'_19633':'_19629');const p={model,kind,source,sha:PINS[model][kind].sha};assert.equal(chooseOperation(id('ota-'+name+'_900'),p).mode,'resume');assert.throws(()=>chooseOperation(id('ota-'+name+'_900'),{...p,model:model==='pro'?'elite':'pro'}));assert.throws(()=>chooseOperation(id('ota-'+name+'_900'),{...p,sha:'bad'}));}
}
for(const text of ['FM-1_016','MKE-P37_016','SMK-37 Pro_19629','SMK-37 Elite_19633'])assert.throws(()=>chooseOperation(id(text)));
let current,fail=false,queries=0,writes=0,foreignLoader=false;
class U{constructor(a,l,o){this.guard=o?.guard;}async find(){queries++;return {id:id(current),link:{close(){}}};}async install(image,product){writes++;this.guard('start',id(current));const name=product.split('_')[0];this.guard('write',id('ota-'+(foreignLoader?(name==='SMK-37 Pro'?'SMK-37 Elite':'SMK-37 Pro'):name)+'_900'));if(fail)throw Error('cut');current=product;}async resume(image,step,{product}){writes++;this.guard('write',id(current));if(fail)throw Error('cut again');current=product;return true;}}
for(const model of ['pro','elite'])for(const kind of ['release','update']){
 const map=new Map(),storage={getItem:k=>map.get(k),setItem:(k,v)=>map.set(k,v),removeItem:k=>map.delete(k)};const service=new InstallService({},storage,()=>{},U),name=PINS[model].model;current=name+(kind==='release'?'_016':model==='pro'?'_19633':'_19629');const source=current;
 let choice=chooseOperation(id(current));const bad={...buffers,[model]:{...buffers[model],release:buffers[model].release.slice(1)}};const q=queries;await assert.rejects(()=>service.execute(choice,bad));assert.equal(queries,q);
 fail=true;await assert.rejects(()=>service.execute(choice,buffers));assert.equal(readPending(storage).source,source);current='ota-'+name+'_900';choice=chooseOperation(id(current),readPending(storage));await assert.rejects(()=>service.execute(choice,buffers));assert.equal(readPending(storage).source,source);fail=false;await service.execute(choice,buffers);assert(!map.has(PENDING));assert.equal(current,name+'_19634');
 // Device substitution after confirmation must refuse before any write.
 choice=chooseOperation(id(current));current=(model==='pro'?'SMK-37 Elite':'SMK-37 Pro')+'_19634';let w=writes;await assert.rejects(()=>service.execute(choice,buffers));assert.equal(writes,w);
 current=name+'_19634';choice=chooseOperation(id(current));foreignLoader=true;await assert.rejects(()=>service.execute(choice,buffers));foreignLoader=false;
}
console.log('PASS: all four model-specific packages, tamper/cross-model refusal, automatic install/update, audited experimental origins, all four interrupted/resumed paths, preserved original source, device substitution refusal before writing and foreign-loader rejection. No hardware accessed.');
