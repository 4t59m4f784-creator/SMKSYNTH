// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';
const lab=new URL('../web/voice-lab/',import.meta.url);const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',lab)).toString('base64'));const wasm=fs.readFileSync(new URL('obervave-voice.wasm',lab));let e=new Engine(wasm);e.advance(5000);
const button=b=>{e.input({t:'btn',b,down:true});e.advance(120);e.input({t:'btn',b,down:false});e.advance(180);};
const enc=(n,steps=1)=>{e.input({t:'enc',e:n,steps});e.advance(100);};
const pad=(p,v)=>{e.input({t:'pad',p,v});e.advance(100);};const key=(k,down)=>{e.input({t:'key',k,down});e.advance(100);};
assert.equal(e.frame().keyboardRow,0);button(6);for(let t=0;t<4;t++){if(t)enc(5);enc(0,-20);}enc(5,-3);button(12);
button(0);assert.equal(e.frame().bank,3);
pad(0,100);pad(1,100);pad(0,0);pad(1,0);assert.equal(e.frame().keyboardMode,2);
key(0,true);assert(e.ex.sim_gates_part(0,0)&&e.ex.sim_gates_part(1,0));
pad(10,100);pad(11,100);pad(10,0);pad(11,0);pad(13,100);pad(13,0);enc(1,-10);
assert.equal(e.frame().keyboardRow,0);assert.equal(e.frame().keyboardEditRow,1);assert.equal(e.frame().keyboardMode,2);
assert.equal(e.frame().pads[7],0xffffff);assert.notEqual(e.frame().pads[15],0xffffff);
pad(15,100);pad(15,0);assert.equal(e.frame().keyboardRow,1);assert.equal(e.frame().keyboardMode,1);assert.equal(e.frame().pads[15],0xffffff);assert.notEqual(e.frame().pads[7],0xffffff);assert.deepEqual(e.frame().keyboardParts,[2,3]);assert.deepEqual(e.frame().keyboardLevels,[70,100]);
key(0,false);assert.equal(e.ex.sim_gates_part(0,0),0);assert.equal(e.ex.sim_gates_part(1,0),0);
key(0,true);assert(e.ex.sim_gates_part(2,0)&&!e.ex.sim_gates_part(3,0));key(0,false);key(12,true);assert(!e.ex.sim_gates_part(2,0)&&e.ex.sim_gates_part(3,0));key(12,false);
for(let i=0;i<4&&e.frame().bank!==2;i++)button(0);const sp=e.frame().pattern-1;e.input({t:'pad',p:sp,v:100});e.advance(850);e.input({t:'pad',p:sp,v:0});e.advance(100);if(e.frame().patternConfirm){pad(sp,100);pad(sp,0);}e.advance(7000);const saved=e.store(true);e=new Engine(wasm,saved);e.advance(5000);assert.equal(e.frame().keyboardRow,1);assert.equal(e.frame().keyboardMode,1);assert.deepEqual(e.frame().keyboardParts,[2,3]);assert.deepEqual(e.frame().keyboardLevels,[70,100]);button(0);pad(7,100);pad(7,0);assert.equal(e.frame().keyboardMode,2);assert.deepEqual(e.frame().keyboardParts,[0,1]);
console.log('PASS: real WASM two-row gestures, inactive editing, max-white selectors, note-off after switch, split boundaries, both configs + selected row after manual save/reboot.');
