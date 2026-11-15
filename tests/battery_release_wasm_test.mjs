// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(process.argv[2]),seed=fs.readFileSync('web/simulator/factory-session.bin');
const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=n=>{e.input({t:'btn',b:n,down:true});e.advance(60);e.input({t:'btn',b:n,down:false});e.advance(160);};
assert.equal(e.ex.sim_battery_level(),0);e.ex.sim_battery_raw(800);e.advance(3500);assert.equal(e.ex.sim_battery_level(),4);
for(let i=0;i<9&&e.ex.sim_page()!==31;i++)b(10);assert.equal(e.ex.sim_page(),31);
const before=e.store(true);e.input({t:'enc',e:0,steps:1});e.advance(2200);assert.equal(e.ex.sim_zoom_theme(),1);
e.ex.sim_battery_raw(0);e.advance(1100);assert.equal(e.ex.sim_battery_level(),0);
e.ex.sim_battery_raw(520);e.advance(3200);assert.equal(e.ex.sim_battery_level(),1);
// Default GLOBE flow still reaches MIDI in seven presses, not a new INFO page.
const re=new Engine(wasm,before);re.advance(5000);for(let i=0;i<7;i++){re.input({t:'btn',b:10,down:true});re.advance(60);re.input({t:'btn',b:10,down:false});re.advance(160);}assert.equal(re.ex.sim_page(),30);
assert(wasm.includes(Buffer.from('Endless MIDI')));assert(wasm.includes(Buffer.from(process.argv[3]||'1.2.0-ema')));
console.log('PASS: real main-loop battery updates, unknown/full/low readings, theme persistence, release strings and unchanged GLOBE navigation.');
