// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import crypto from 'node:crypto';
import {Engine} from '../web/simulator/engine.js';
const candidate=fs.readFileSync(process.argv[2]||'build/controls/SMKSYNTH-1.0.1-ct1.wasm');
const base=fs.readFileSync('web/simulator/obervave-voice.wasm');const seed=fs.readFileSync('factory/simulator-session.bin');
const fresh=wasm=>{const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);return e;};
const button=(e,b)=>{e.input({t:'btn',b,down:true});e.advance(100);e.input({t:'btn',b,down:false});e.advance(120);};
const enc=(e,k,steps)=>{e.input({t:'enc',e:k,steps});e.advance(120);};
const pad=(e,p)=>{e.input({t:'pad',p,v:100});e.advance(80);e.input({t:'pad',p,v:0});e.advance(120);};
let e=fresh(candidate);assert.equal(e.ex.sim_fader_jump(),0);const attack=e.ex.sim_param(0,24);
const cutoff=e.ex.sim_param(0,12);e.input({t:'fader',f:0,v:cutoff});e.advance(100);e.input({t:'fader',f:0,v:Math.min(cutoff+4,127)});e.advance(100);assert(e.ex.sim_fader_caught(0));assert.equal(e.ex.sim_param(0,24),attack);
for(let i=0;i<6&&e.ex.sim_page()!==28;i++)button(e,10);assert.equal(e.ex.sim_page(),28);enc(e,0,-1);assert.equal(e.ex.sim_fader_jump(),1);
// The original preset BT save also persists the global setting; no new auto-save path.
button(e,11);e.advance(5000);const saved=e.store(true);e=new Engine(candidate,saved);e.advance(5000);assert.equal(e.ex.sim_fader_jump(),1);assert.equal(e.frame().used,255);assert.equal(e.frame().patternsUsed,31);
while(e.frame().bank!==2)button(e,0);pad(e,1);assert.equal(e.ex.sim_fader_jump(),1,'pattern recall does not overwrite global mode');
// A page transition with K1 already queued must not apply that delta to its new target.
e=fresh(candidate);const prior=e.ex.sim_param(0,20);e.input({t:'enc',e:0,steps:1});e.input({t:'btn',b:9,down:true});e.advance(100);e.input({t:'btn',b:9,down:false});e.advance(120);assert.equal(e.ex.sim_page(),6);assert.equal(e.ex.sim_param(0,20),prior);
enc(e,0,1);assert.equal(e.ex.sim_param(0,20),Math.min(prior+2,127));
// Existing curated patterns render exactly the same samples; fader logic adds no DSP changes.
const hashes=[];for(let slot=0;slot<5;slot++){
 const engines=[fresh(base),fresh(candidate)],digests=[];
 for(const x of engines){while(x.frame().bank!==2)button(x,0);pad(x,slot);button(x,16);const hash=crypto.createHash('sha256');for(let n=0;n<240;n++){x.render(256);hash.update(new Uint8Array(x.ex.memory.buffer,x.ex.sim_out_l(),1024));hash.update(new Uint8Array(x.ex.memory.buffer,x.ex.sim_out_r(),1024));}digests.push(hash.digest('hex'));}
 assert.equal(digests[0],digests[1],'PCM changed for factory pattern '+(slot+1));hashes.push(digests[0]);
}
console.log(JSON.stringify({result:'PASS',global_mode_save_reboot:true,pattern_keeps_mode:true,queued_page_delta_rejected:true,next_detent_relative:true,factory_patterns_pcm_bit_identical:5,hashes}));
