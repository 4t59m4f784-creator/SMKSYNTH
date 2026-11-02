// SPDX-License-Identifier: GPL-3.0-only
// Exercise actual controller in a DOM/MIDI-service harness; never calls USB.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';
import {PINS,validate,modeFor,readPending} from '../web/keyboard-ad1/service.js';
class Element{
 constructor(){this.events={};this.disabled=false;this.textContent='';this.returnValue='';}
 addEventListener(k,fn){this.events[k]=fn}removeAttribute(){}showModal(){this.open=true}
}
const nodes=new Map();const elem=k=>{if(!nodes.has(k))nodes.set(k,new Element());return nodes.get(k)};
let midi=0,writes=0,fetches=0;
class Service{async identify(){return {text:'SMK-37 Pro_19552',model:'SMK-37 Pro'}}async execute(){writes++;return 'SMK-37 Pro_19605'}}
const code=fs.readFileSync(new URL('../web/keyboard-ad1/controller.js',import.meta.url),'utf8').replace(/^import .*\n/m,'');
const ctx={InstallService:Service,PINS,validate,modeFor,readPending,document:{getElementById:elem},localStorage:{getItem(){return null}},window:{addEventListener(){}},navigator:{async requestMIDIAccess(){midi++;return {}},locks:{async request(k,o,fn){return fn({})}}},fetch:async url=>{fetches++;const bytes=fs.readFileSync(new URL('../web/keyboard-ad1/'+url,import.meta.url));return {ok:true,arrayBuffer:async()=>bytes}},Date,Uint8Array,console,setTimeout};
vm.runInNewContext(code,ctx);
for(let i=0;i<100&&!elem('packageStatus').textContent.includes('aprovados');i++)await new Promise(r=>setTimeout(r,10));
assert.equal(fetches,4);assert.match(elem('packageStatus').textContent,/aprovados/);assert.equal(midi,0);assert.equal(writes,0);
for(const k of ['ad1','pf12','pf9','vd4'])assert(elem(k).disabled);
await elem('connect').events.click();assert.equal(midi,1);for(const k of ['ad1','pf12','pf9','vd4'])assert(!elem(k).disabled);
await elem('ad1').events.click();assert(elem('confirmation').open);assert.equal(writes,0);assert.match(elem('confirmDetails').textContent,/19605/);
elem('confirmation').returnValue='cancel';await elem('confirmation').events.close();assert.equal(writes,0);
await elem('ad1').events.click();elem('confirmation').returnValue='confirm';await elem('confirmation').events.close();assert.equal(writes,1);assert.match(elem('operationStatus').textContent,/Concluído.*19605/);
console.log('PASS: actual controller fetches/verifies all packages, no MIDI access on load, hardware buttons gated, explicit connect/dialog/cancel/confirm, lock and final identity state. DOM and device service mocked; no physical USB.');
