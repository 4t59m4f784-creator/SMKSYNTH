import fs from 'node:fs';import assert from 'node:assert/strict';
const lab=new URL('../web/voice-lab/',import.meta.url);const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',lab)).toString('base64'));const wasm=fs.readFileSync(new URL('obervave-voice.wasm',lab));let e=new Engine(wasm);e.advance(5000);
const b=(n,ms=120)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(200);};const enc=(n,steps)=>{e.input({t:'enc',e:n,steps});e.advance(100);};const pad=(p,ms=100)=>{e.input({t:'pad',p,v:100});e.advance(ms);e.input({t:'pad',p,v:0});e.advance(100);};
// Preset-only BT on SEQ, including explicit save-as destination and cancel.
b(15);const before=e.ex.sim_writes(2);b(11);assert.equal(e.ex.sim_pattern_confirm(),0);e.advance(6500);assert.equal(e.ex.sim_writes(2),before);assert(e.ex.sim_writes(1)>0);
b(11,900);enc(6,20);b(12);assert.notEqual(e.frame().programs[e.frame().track],21);b(11,900);enc(6,20);b(11);e.advance(6500);assert.equal(e.frame().programs[e.frame().track],21);assert.equal(e.ex.sim_writes(2),before);
for(const n of [6,10,13,14,15]){b(n);const pw=e.ex.sim_writes(1);b(11);assert.equal(e.frame().presetSave,2);e.advance(6500);assert(e.ex.sim_writes(1)>pw);assert.equal(e.ex.sim_writes(2),before);}
// Only pad bank can save the pattern. Snapshot selected D and row1 LAYER.
b(0);b(0);assert.equal(e.frame().bank,3);pad(6);for(let i=0;i<3;i++)enc(5,1);assert.equal(e.frame().track,3);b(0);assert.equal(e.frame().bank,2);
const slot=e.frame().pattern-1;pad(slot,850);if(e.frame().patternConfirm){assert.equal(e.frame().patternConfirm,slot+1);pad(slot);}e.advance(7000);assert(e.ex.sim_writes(2)>before);
// Existing current pattern asks a distinct tap; release of initial hold cannot confirm.
const writes=e.ex.sim_writes(2);enc(5,-1);enc(5,-1);assert.equal(e.frame().track,1);pad(slot,850);assert.equal(e.frame().patternConfirm,slot+1);e.advance(4100);assert.equal(e.frame().patternConfirm,0);e.advance(3000);assert.equal(e.ex.sim_writes(2),writes);
pad(slot);assert.equal(e.frame().track,3);assert.equal(e.frame().keyboardMode,2);
// Confirm overwrites focus, and it survives full session reboot with keyboard unchanged.
enc(5,-1);assert.equal(e.frame().track,2);pad(slot,850);assert(e.frame().patternConfirm);pad(slot);e.advance(7000);assert(e.ex.sim_writes(2)>writes);
const saved=e.store(true);e=new Engine(wasm,saved);e.advance(5000);assert.equal(e.frame().track,2);assert.equal(e.frame().keyboardMode,2);
console.log('PASS: real WASM BT preset-only on SEQ, long save-as/cancel, slot chosen + preset writes, pad-only pattern save, separate tap overwrite, 4s timeout/no writes, D/C selected part recall and reboot, LAYER retained.');
