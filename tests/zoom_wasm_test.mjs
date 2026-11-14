// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(process.argv[2]),seed=fs.readFileSync('web/simulator/factory-session.bin');
const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=(n,hold=60)=>{e.input({t:'btn',b:n,down:true});e.advance(hold);e.input({t:'btn',b:n,down:false});e.advance(160);};
const enc=(n,v)=>{e.input({t:'enc',e:n,steps:v});e.advance(160);};
const patches=()=>Array.from({length:4},(_,t)=>Array.from({length:80},(_,p)=>e.ex.sim_param(t,p)));
const shot=name=>{const fb=new Uint16Array(e.ex.memory.buffer,e.ex.sim_fb(),57600),out=Buffer.alloc(172800);for(let i=0;i<fb.length;i++){const v=fb[i];out[i*3]=(v>>11&31)*255/31;out[i*3+1]=(v>>5&63)*255/63;out[i*3+2]=(v&31)*255/31;}fs.writeFileSync('build/zm1/'+name+'.ppm',Buffer.concat([Buffer.from('P6\n240 240\n255\n'),out]));};
assert.equal(e.ex.sim_zoom_theme(),0);assert.equal(e.ex.sim_scope_view(),0);const before=patches();
// Real GLOBE page is appended, with a safe Obervave default.
for(let i=0;i<9&&e.ex.sim_page()!==31;i++)b(10);assert.equal(e.ex.sim_page(),31);shot('wasm-theme-default');enc(0,1);assert.equal(e.ex.sim_zoom_theme(),1);e.advance(2200);assert.equal(e.ex.sim_theme_pending(),0);assert.deepEqual(patches(),before);
const saved=e.store(true),re=new Engine(wasm,saved);re.advance(5000);assert.equal(re.ex.sim_zoom_theme(),1);assert.equal(re.ex.sim_scope_view(),0);
b(12,800);assert.equal(e.ex.sim_page(),0);assert.equal(e.ex.sim_scope_view(),0);shot('wasm-perform');
b(12);assert.equal(e.ex.sim_page(),0);assert.equal(e.ex.sim_scope_view(),1);shot('wasm-scope');
const old=e.ex.sim_param(0,12);enc(0,1);assert.equal(e.ex.sim_zoom_focus(),1);assert.notEqual(e.ex.sim_param(0,12),old);shot('wasm-scope-focus');e.advance(1150);assert.equal(e.ex.sim_zoom_focus(),0);assert.equal(e.ex.sim_scope_view(),1);
e.input({t:'key',k:12,down:true});e.advance(300);assert(e.ex.sim_peak()>0);assert(e.ex.sim_melodic_busy()>0);shot('wasm-scope-audio');e.input({t:'key',k:12,down:false});
b(12);assert.equal(e.ex.sim_page(),19);assert.equal(e.ex.sim_mix_row(),0);for(let i=1;i<=7;i++){b(12);assert.equal(e.ex.sim_mix_row(),i%7);}b(12,800);assert.equal(e.ex.sim_page(),0);assert.equal(e.ex.sim_scope_view(),0);
// The new scope is shared by both themes; selecting Obervave preserves data.
for(let i=0;i<9&&e.ex.sim_page()!==31;i++)b(10);assert.equal(e.ex.sim_page(),31);const after=patches();enc(0,-1);assert.equal(e.ex.sim_zoom_theme(),0);assert.deepEqual(patches(),after);b(12,800);b(12);assert.equal(e.ex.sim_scope_view(),1);b(12);assert.equal(e.ex.sim_page(),19);b(12,800);assert.equal(e.ex.sim_scope_view(),0);
enc(4,3);b(12);assert.equal(e.ex.sim_scope_view(),1);assert.equal(e.ex.sim_zoom_focus(),0,'Opening scope after an outside edit must not create a stale notice');b(12,800);
console.log('PASS: real GLOBE theme default/selection/persistence, unchanged four patches, short/long PATCH scope/mixer flow in both themes, live audio and one-second popup expiry.');
