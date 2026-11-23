// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync('web/simulator/obervave-voice.wasm'),seed=fs.readFileSync('web/simulator/factory-session.bin');
const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=(n,hold=60)=>{e.input({t:'btn',b:n,down:true});e.advance(hold);e.input({t:'btn',b:n,down:false});e.advance(160);};
const enc=(n,v)=>{e.input({t:'enc',e:n,steps:v});e.advance(160);};
const tempo=()=>{const p=e.frame().pattern,n=e.ex.sim_tempo();enc(4,3);assert.equal(e.ex.sim_tempo(),n+3);assert.equal(e.frame().pattern,p);};
tempo();b(12);tempo();b(12);assert.equal(e.ex.sim_page(),19);tempo();b(12,800);b(15);tempo();b(12,800);
b(13);assert.equal(e.ex.sim_page(),1);b(13);assert.equal(e.ex.sim_page(),2);b(13);assert.equal(e.ex.sim_page(),32);tempo();enc(2,20);e.input({t:'key',k:12,down:true});e.advance(300);assert(e.ex.sim_peak()>0);e.input({t:'key',k:12,down:false});e.advance(1500);
for(let i=0;i<14&&e.ex.sim_page()!==34;i++)b(10);assert.equal(e.ex.sim_page(),34);tempo();
for(let i=0;i<14&&e.ex.sim_page()!==33;i++)b(10);assert.equal(e.ex.sim_page(),33);b(16);enc(0,1);b(16);assert.equal(e.ex.sim_page(),33);e.advance(2600);assert.equal(e.ex.sim_page(),0);
console.log('PASS 1.3.0 WASM: K5 tempo on Performance/scope/Mixer/SEQ/SUB/Global, pattern unchanged; real SUB page/audio; new TEMPO SOURCE and INITIALIZE pages; confirmed initialization automatically returns to Performance.');
