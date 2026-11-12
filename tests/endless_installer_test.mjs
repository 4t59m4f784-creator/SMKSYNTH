// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {webcrypto} from 'node:crypto';if(!globalThis.crypto)globalThis.crypto=webcrypto;
import {PINS,PENDING,validate,modeFor,InstallService} from '../experimental/endless/service.js';import {PINS as stable} from '../web/install/service.js';
const id=text=>({text,model:text.split('_')[0]});const buffers={};
for(const [k,p]of Object.entries(PINS)){buffers[k]=new Uint8Array(fs.readFileSync('experimental/endless/'+p.url.slice(2)));await validate(buffers[k],k);const bad=buffers[k].slice();bad[1000]^=1;await assert.rejects(()=>validate(bad,k));}
for(const src of ['19618','19619','19620','19621','19622','19623','19624','19625','19626','19627','19628','19630','19631','19632','19633','19634','19635'])assert.equal(modeFor('update',id('SMK-37 Pro_'+src)),'install');
for(const src of ['19619','19620','19621','19622','19623','19624','19625','19626','19627','19628','19630','19631','19632','19633','19634','19635'])assert.equal(modeFor('release',id('SMK-37 Pro_'+src)),'install');
for(const k of Object.keys(PINS)){for(const src of ['SMK-37 Pro_016','SMK-37 Elite_19618','FM-1_19618'])assert.throws(()=>modeFor(k,id(src)));assert.equal(modeFor(k,id('ota-SMK-37 Pro_900'),{kind:k,sha:PINS[k].sha,source:'SMK-37 Pro_19623'}),'resume');assert.throws(()=>modeFor(k,id('ota-SMK-37 Pro_900'),{kind:k,sha:'bad',source:'SMK-37 Pro_19623'}));}
assert.equal(PINS.release.product,'SMK-37 Pro_19634');assert(PENDING.includes('super.sp2'));
let source='SMK-37 Pro_19622',fail=true;
class U{constructor(a,l,o){this.o=o;}async find(){return{id:id(source),link:{close(){}}};}async install(image,product){this.o.guard('start',id(source));this.o.guard('write',id('ota-SMK-37 Pro_900'));if(fail)throw Error('cut');source=product;}async resume(image,s,{product}){this.o.guard('write',id(source));if(fail)throw Error('cut again');source=product;return true;}}
const m=new Map(),storage={getItem:k=>m.get(k),setItem:(k,v)=>m.set(k,v),removeItem:k=>m.delete(k)},svc=new InstallService({},storage,()=>{},U);
await assert.rejects(()=>svc.execute('update',buffers));source='ota-SMK-37 Pro_900';await assert.rejects(()=>svc.execute('update',buffers));assert.equal(JSON.parse(m.get(PENDING)).source,'SMK-37 Pro_19622');fail=false;await svc.execute('update',buffers);assert.equal(source,'SMK-37 Pro_19635');await svc.execute('release',buffers);assert.equal(source,'SMK-37 Pro_19634');
console.log('PASS: pinned SP2/official 1.1.0 return, strict model/origin/hash guards, two interrupted resumes preserve source, UI1->SP2->official.');

// Interrupted E1 remains recoverable with its original immutable package/key.
const legacy=await import('../experimental/endless/legacy-e1/service.js');
assert(legacy.PENDING.includes('endless.e1'));assert.notEqual(legacy.PENDING,PENDING);
for(const [k,p] of Object.entries(legacy.PINS)){const raw=new Uint8Array(fs.readFileSync('experimental/endless/legacy-e1/'+p.url.slice(2)));await legacy.validate(raw,k);assert.equal(legacy.modeFor(k,id('ota-SMK-37 Pro_900'),{kind:k,sha:p.sha,source:'SMK-37 Pro_19622'}),'resume');}
assert.equal(legacy.PINS.update.product,'SMK-37 Pro_19623');assert.equal(legacy.PINS.release.sha,stable.update.sha);console.log('PASS: frozen E1 packages, resume identity and pending key preserved.');

const e2=await import('../experimental/endless/legacy-e2/service.js');
assert(e2.PENDING.includes('endless.e2'));assert.notEqual(e2.PENDING,PENDING);
for(const [k,p] of Object.entries(e2.PINS)){await e2.validate(new Uint8Array(fs.readFileSync('experimental/endless/legacy-e2/'+p.url.slice(2))),k);assert.equal(e2.modeFor(k,id('ota-SMK-37 Pro_900'),{kind:k,sha:p.sha,source:'SMK-37 Pro_19624'}),'resume');assert.equal(e2.modeFor(k,id('SMK-37 Pro_19625')),'install');}
assert.equal(e2.PINS.update.product,'SMK-37 Pro_19624');console.log('PASS: original E2 package and pending key, interrupted resume and direct E3 -> E2 comparison.');

const e3=await import('../experimental/endless/legacy-e3/service.js');assert(e3.PENDING.includes('endless.e3'));assert.notEqual(e3.PENDING,PENDING);
for(const [k,p] of Object.entries(e3.PINS)){await e3.validate(new Uint8Array(fs.readFileSync('experimental/endless/legacy-e3/'+p.url.slice(2))),k);assert.equal(e3.modeFor(k,id('ota-SMK-37 Pro_900'),{kind:k,sha:p.sha,source:'SMK-37 Pro_19625'}),'resume');assert.equal(e3.modeFor(k,id('SMK-37 Pro_19626')),'install');}
assert.equal(e3.PINS.update.product,'SMK-37 Pro_19625');console.log('PASS: original E3 package/key, interrupted resume and direct E4 -> E3 comparison.');

const e4=await import('../experimental/endless/legacy-e4/service.js');assert(e4.PENDING.includes('endless.e4'));assert.notEqual(e4.PENDING,PENDING);
for(const [k,p] of Object.entries(e4.PINS)){await e4.validate(new Uint8Array(fs.readFileSync('experimental/endless/legacy-e4/'+p.url.slice(2))),k);assert.equal(e4.modeFor(k,id('ota-SMK-37 Pro_900'),{kind:k,sha:p.sha,source:'SMK-37 Pro_19626'}),'resume');assert.equal(e4.modeFor(k,id('SMK-37 Pro_19627')),'install');}
assert.equal(e4.PINS.update.product,'SMK-37 Pro_19626');console.log('PASS: original E4 package/key, interrupted resume and direct E5 -> E4 comparison.');

const e5=await import('../experimental/endless/legacy-e5/service.js');assert(e5.PENDING.includes('endless.e5'));assert.notEqual(e5.PENDING,PENDING);
for(const [k,p] of Object.entries(e5.PINS)){await e5.validate(new Uint8Array(fs.readFileSync('experimental/endless/legacy-e5/'+p.url.slice(2))),k);assert.equal(e5.modeFor(k,id('ota-SMK-37 Pro_900'),{kind:k,sha:p.sha,source:'SMK-37 Pro_19627'}),'resume');assert.equal(e5.modeFor(k,id('SMK-37 Pro_19628')),'install');}
assert.equal(e5.PINS.update.product,'SMK-37 Pro_19627');console.log('PASS: original E5 package/key, interrupted resume and direct SP1 -> E5 comparison.');

const sp1=await import('../experimental/endless/legacy-sp1/service.js');assert(sp1.PENDING.includes('super.sp1'));assert.notEqual(sp1.PENDING,PENDING);assert.equal(sp1.modeFor('update',id('SMK-37 Pro_19635')),'install');
for(const [kind,pin] of Object.entries(sp1.PINS))await sp1.validate(new Uint8Array(fs.readFileSync('experimental/endless/legacy-sp1/'+pin.url.slice(2))),kind);

// M1.1 -> SP2: connection, both interrupted retries and source-bound resume.
for(const [kind,target]of [['update','SMK-37 Pro_19635'],['release','SMK-37 Pro_19634']]){
 m.clear();source='SMK-37 Pro_19632';fail=true;
 await assert.rejects(()=>svc.execute(kind,buffers));assert.equal(JSON.parse(m.get(PENDING)).source,'SMK-37 Pro_19632');
 source='ota-SMK-37 Pro_900';await assert.rejects(()=>svc.execute(kind,buffers));assert.equal(JSON.parse(m.get(PENDING)).source,'SMK-37 Pro_19632');
 fail=false;await svc.execute(kind,buffers);assert.equal(source,target);assert.equal(m.size,0);
}
for(const sourceId of ['19636','19632_extra','1963'])for(const kind of ['update','release'])assert.throws(()=>modeFor(kind,id('SMK-37 Pro_'+sourceId)));
console.log('PASS: PED1/M1/M1.1/M1.2 accepted explicitly, actual M1.1 install/return interruption/resume retains original source, unknown identities still refused.');
