// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';
const lab=new URL('../web/voice-lab/',import.meta.url);const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',lab)).toString('base64'));const wasm=fs.readFileSync(new URL('obervave-voice.wasm',lab));let e=new Engine(wasm);e.advance(5000);
const b=(n,ms=100)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(100);};
const pad=(p,ms=100)=>{e.input({t:'pad',p,v:100});e.advance(ms);e.input({t:'pad',p,v:0});e.advance(100);};
const enc=(n,steps)=>{e.input({t:'enc',e:n,steps});e.advance(100);};
b(0);b(0);assert.equal(e.frame().bank,2);
for(let p=0;p<16;p++){
 enc(5,1);const focus=e.frame().track;
 pad(p,850);if(e.frame().patternConfirm)pad(p);e.advance(6000);
 assert(e.frame().patternsUsed&(1<<p));
 pad(p);assert.equal(e.frame().pattern,p+1);assert.equal(e.frame().track,focus);
}
assert.equal(e.frame().patternsUsed,65535);
const before=e.ex.sim_writes(2);pad(15,850);assert.equal(e.frame().patternConfirm,16);e.advance(4100);assert.equal(e.frame().patternConfirm,0);e.advance(3000);assert.equal(e.ex.sim_writes(2),before);
pad(15,850);pad(15);e.advance(6000);assert(e.ex.sim_writes(2)>before);
const saved=e.store(true);assert.equal(saved.byteLength,19*16384);e=new Engine(wasm,saved);e.advance(5000);assert.equal(e.frame().patternsUsed,65535);assert.equal(e.frame().pattern,16);
// Run pattern1, queue16 through the actual pads, commit at the existing downbeat.
b(0);b(0);pad(0);assert.equal(e.frame().pattern,1);b(16);b(0);b(0);b(0);assert.equal(e.frame().bank,2);pad(15);assert.equal(e.frame().patternQueue,16);e.advance(2500);assert.equal(e.frame().pattern,16);b(16);b(0);b(0);b(0);assert.equal(e.frame().bank,2);
// A real U2-compatible session has 1 global/program block +8 pattern blocks +2 favorite blocks.
pad(0);const current=new Uint8Array(e.store(true)),s=16384,old=new Uint8Array(11*s);old.set(current.subarray(0,9*s));old.set(current.subarray(17*s,19*s),9*s);
const migrated=new Engine(wasm,old.buffer);migrated.advance(5000);assert.equal(migrated.frame().patternsUsed,255);
const after=new Uint8Array(migrated.store(true));assert.deepEqual(after.subarray(s,9*s),old.subarray(s,9*s));assert.deepEqual(after.subarray(17*s,19*s),old.subarray(9*s,11*s));assert(after.subarray(9*s,17*s).every(v=>v===0));
assert.throws(()=>new Engine(wasm,new ArrayBuffer(12*s)),/incompatível/);
console.log('PASS: real WASM save/load 16 pads + selected parts, all16 occupancy, pad16 confirmation +4s timeout/no writes, 19-block reboot, old11-block remap preserving eight slot/favorite bytes, new slots empty, synchronized slot16 switch, unsupported size rejected.');
