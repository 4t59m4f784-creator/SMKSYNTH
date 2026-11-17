import fs from 'node:fs';import vm from 'node:vm';import assert from 'node:assert/strict';import {webcrypto} from 'node:crypto';
import {PINS,validate,modeFor,readPending,pinKey,modelKey} from '../experimental/dual-core-dc4/service.js';
if(!globalThis.crypto)globalThis.crypto=webcrypto;
const source=fs.readFileSync('experimental/dual-core-dc4/controller.js','utf8').replace(/^import .*$/gm,'').replace('refresh();fetchPackages();','refresh();globalThis.hooks={fetchPackages};');
for(const model of ['Pro','Elite']){
 const elements=new Map();const get=id=>{if(!elements.has(id))elements.set(id,{disabled:false,textContent:'',listeners:{},addEventListener(n,f){this.listeners[n]=f},showModal(){this.open=true}});return elements.get(id)};
 const c={PINS,validate,modeFor,readPending,pinKey,modelKey,InstallService:class{async identify(){return {model:'SMK-37 '+model,text:'SMK-37 '+model+'_19660'}}},document:{getElementById:get},navigator:{requestMIDIAccess:async()=>({})},localStorage:{getItem:()=>null},window:{addEventListener(){}},Date,console,
 fetch:async path=>({ok:true,arrayBuffer:async()=>{const b=fs.readFileSync('experimental/dual-core-dc4/'+path);return b.buffer.slice(b.byteOffset,b.byteOffset+b.byteLength)}})};
 vm.createContext(c);vm.runInContext(source,c);await c.hooks.fetchPackages();await get('connect').listeners.click();
 for(const kind of ['update','baseline','release']){assert(!get(kind).disabled);get(kind).listeners.click();assert.equal(get('confirmTitle').textContent,PINS[model.toLowerCase()+'.'+kind].name);assert(get('confirmDetails').textContent.includes(PINS[model.toLowerCase()+'.'+kind].product));assert(get('confirmation').open);}
}
console.log('PASS: real DC4 controller loads all six packages and wires update, DC3 baseline and official return confirmations for Pro/Elite; no hardware accessed.');
