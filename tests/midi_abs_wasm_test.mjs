// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(process.argv[2]||'build/mm11-sim/SMKSYNTH-1.1.0-mm1a.wasm');const e=new Engine(wasm);e.advance(5000);
const b=n=>{e.input({t:'btn',b:n,down:true});e.advance(60);e.input({t:'btn',b:n,down:false});e.advance(100);};
const turn=(k,steps,ms=15)=>{e.input({t:'enc',e:k,steps});e.advance(ms);};
for(let n=0;n<8&&e.ex.sim_page()!==30;n++)b(10);assert.equal(e.ex.sim_page(),30);turn(0,1,400);assert.equal(e.ex.sim_midi_mode(),1);
const last=()=>{let n=e.ex.sim_midi_tx_count()-1;return [0,1,2].map(f=>e.ex.sim_midi_tx(n,f));};
for(let n=0;n<17;n++)turn(0,1);assert.deepEqual(last(),[0xB0,70,127]);turn(0,-1);assert.deepEqual(last(),[0xB0,70,126]);e.advance(200);turn(0,-1);assert.deepEqual(last(),[0xB0,70,125]);
// Speed in the other encoder starts with a single unit, not the previous gain.
turn(7,1);assert.deepEqual(last(),[0xB0,77,1]);turn(7,1);assert.deepEqual(last(),[0xB0,77,9]);
b(10);turn(3,1,100);b(10);e.advance(400);turn(0,1);assert.deepEqual(last(),[0xB0,70,65]);turn(0,1);assert.deepEqual(last(),[0xB0,70,65]);
console.log('PASS: actual firmware loop, ABS 0->127 in 17 fast steps, reversal126/slow125, independent K8, REL still sends65 on fast +1 turns.');
