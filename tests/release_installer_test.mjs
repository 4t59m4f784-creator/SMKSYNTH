import assert from 'node:assert/strict';import fs from 'node:fs';import {webcrypto} from 'node:crypto';if(!globalThis.crypto)Object.defineProperty(globalThis,'crypto',{value:webcrypto});
import {PINS,PENDING,validate,modeFor,InstallService} from '../web/install/service.js';
const raw=new Uint8Array(fs.readFileSync(new URL('../web/install/'+PINS.release.url,import.meta.url)));await validate(raw,'release');const bad=raw.slice();bad[900]^=1;await assert.rejects(()=>validate(bad,'release'));
const id=text=>({text,model:text.split('_')[0]});assert.equal(modeFor('release',id('SMK-37 Pro_016')),'install');
for(const text of ['SMK-37 Pro_015','SMK-37 Pro_017','SMK-37 Pro_19615','SMK-37 Pro_19617','SMK-37 Pro_19618','SMK-37 Elite_016','MKE-P37_016','FM-1_016','SMK-37_016'])assert.throws(()=>modeFor('release',id(text)));
for(const pending of [null,{kind:'release',sha:PINS.release.sha,source:'SMK-37 Pro_015'},{kind:'release',sha:'bad',source:'SMK-37 Pro_016'}])assert.throws(()=>modeFor('release',id('ota-SMK-37 Pro_900'),pending));
assert.equal(modeFor('release',id('ota-SMK-37 Pro_900'),{kind:'release',sha:PINS.release.sha,source:'SMK-37 Pro_016'}),'resume');
let source='SMK-37 Pro_016',writes=0,fail=false;class U{constructor(a,l,o){this.o=o;}async find(){return{id:id(source),link:{close(){}}};}async install(image,product){this.o.guard('start',id(source));this.o.guard('write',id('ota-SMK-37 Pro_900'));writes++;if(fail)throw Error('cut');source=product;}async resume(image,step,{product}){this.o.guard('write',id(source));writes++;source=product;return true;}}
const map=new Map(),storage={getItem:k=>map.get(k),setItem:(k,v)=>map.set(k,v),removeItem:k=>map.delete(k)},service=new InstallService({},storage,()=>{},U);
source='SMK-37 Pro_19617';await assert.rejects(()=>service.execute('release',{release:raw}));assert.equal(writes,0);
source='SMK-37 Pro_016';fail=true;await assert.rejects(()=>service.execute('release',{release:raw}));assert(map.has(PENDING));source='ota-SMK-37 Pro_900';fail=false;await service.execute('release',{release:raw});assert.equal(source,'SMK-37 Pro_19618');assert(!map.has(PENDING));
console.log('PASS: exact release pin/model/CRC; stock Pro V016 only; other models/versions and old SMKSYNTH refused before write; resume bound to exact package + stock source; target identity confirmed. No physical USB.');
