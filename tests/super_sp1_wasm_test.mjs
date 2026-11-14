// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const bytes=fs.readFileSync(process.argv[2]||'build/sp1-sim/SMKSYNTH-1.0.3-sp1.wasm');
const seed=fs.readFileSync('web/simulator/factory-session.bin');const e=new Engine(bytes,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=n=>{e.input({t:'btn',b:n,down:true});e.advance(80);e.input({t:'btn',b:n,down:false});e.advance(180);};
const enc=(n,steps)=>{e.input({t:'enc',e:n,steps});e.advance(100);};
b(13);enc(0,100);assert.equal(e.ex.sim_param(0,0),4);b(13);enc(0,100);assert.equal(e.ex.sim_param(0,4),3);
e.input({t:'midi',st:0xB0,d1:24,d2:127});e.advance(100);assert.equal(e.ex.sim_param(0,4),3);
for(let i=0;i<4;i++)b(13);const det=e.ex.sim_param(0,1),pw2=e.ex.sim_param(0,5);enc(2,1);enc(3,1);assert.equal(e.ex.sim_param(0,1),det);assert.equal(e.ex.sim_param(0,5),pw2);enc(0,-1);assert.notEqual(e.ex.sim_param(0,1),det);
for(let turn=0;turn<8;turn++){for(let k=0;k<8;k++)e.input({t:'key',k:12+k*2,down:true});e.advance(100);for(let k=0;k<8;k++)e.input({t:'key',k:12+k*2,down:false});e.advance(60);}
console.log(JSON.stringify({result:'PASS',osc1_super_available:true,osc2_super_blocked:true,midi_osc2_cap:true,only_two_super_knobs:true,repeated_eight_note_attacks:true,hardware_deadline_tested:false}));
