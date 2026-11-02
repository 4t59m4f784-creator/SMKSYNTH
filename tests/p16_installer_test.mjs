// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {webcrypto} from 'node:crypto';
if(!globalThis.crypto)Object.defineProperty(globalThis,"crypto",{value:webcrypto});
const {PINS,PENDING,validate,modeFor,InstallService}=await import('../web/keyboard-p16/service.js');
const buffers={};for(const k of Object.keys(PINS)){
 buffers[k]=new Uint8Array(fs.readFileSync(new URL('../web/keyboard-p16/'+PINS[k].url,import.meta.url)));
 assert.equal((await validate(buffers[k],k)).product,PINS[k].product);
 const corrupt=buffers[k].slice();corrupt[700]^=1;await assert.rejects(()=>validate(corrupt,k));
 await assert.rejects(()=>validate(buffers[k].slice(0,-1),k));
}
const id=text=>({text,model:text.split('_')[0]});
for(const k of Object.keys(PINS)){
 assert.equal(modeFor(k,id('SMK-37 Pro_19613')),'install');
 assert.equal(modeFor(k,id('SMK-37 Pro_19602')),'install');
 assert.equal(modeFor(k,id('SMK-37 Pro_19552')),'install');
 assert.equal(modeFor(k,id('SMK-37 Pro_000')),'install');
 assert.throws(()=>modeFor(k,id('FM-1_900')));
 assert.throws(()=>modeFor(k,id('ota-SMK-37 Pro_901')));
 assert.throws(()=>modeFor(k,id('SMK-37 Pro_99999')));
}
assert.throws(()=>modeFor('p16',id('ota-SMK-37 Pro_900')));
assert.throws(()=>modeFor('p16',id('ota-SMK-37 Pro_900'),{kind:'p16',sha:'wrong'}));
assert.equal(modeFor('p16',id('ota-SMK-37 Pro_900'),{kind:'p16',sha:PINS.p16.sha}),'resume');
assert.equal(modeFor('pf12',id('ota-SMK-37 Pro_900')),'resume');
let source='SMK-37 Pro_19552',writes=0,finds=0,fail=false,changed=false;
class FakeUpdater{
 constructor(a,l,opt={}){this.opt=opt}
 async find(){finds++;return {id:id(source),link:{close(){}}}}
 async install(image,product){this.opt.guard('start',id(changed?'SMK-37 Pro_99999':source));this.opt.guard('write',id('ota-SMK-37 Pro_900'));writes++;if(fail)throw Error('interrupted');source=product}
 async resume(image,step,{product}){this.opt.guard('write',id(source));writes++;source=product;return true}
}
const items=new Map(),storage={getItem:k=>items.get(k),setItem:(k,v)=>items.set(k,v),removeItem:k=>items.delete(k)};
const service=new InstallService({},storage,()=>{},FakeUpdater);
for(const k of ['p16','sl1','p16','ad1','p16','pf12','p16','pf9']){assert.equal(await service.execute(k,buffers),PINS[k].product);assert(!items.has(PENDING))}
source='SMK-37 Pro_19552';fail=true;await assert.rejects(()=>service.execute('p16',buffers));assert(items.has(PENDING));
fail=false;source='ota-SMK-37 Pro_900';assert.equal(await service.execute('p16',buffers),PINS.p16.product);
changed=true;await assert.rejects(()=>service.execute('pf12',buffers));changed=false;
const before=finds,bad={...buffers,pf9:buffers.pf9.slice(1)};await assert.rejects(()=>service.execute('p16',bad));assert.equal(finds,before);
console.log('PASS: nine pinned packages, corruption/truncation, strict model/identity, changed identity guard, install/return AD1/PF12/PF9, interrupted update/resume, pending cleanup, all buffers checked before MIDI. USB transport not exercised.');
