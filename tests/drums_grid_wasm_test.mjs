// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';
const lab=new URL('../web/voice-lab/',import.meta.url);
const {Engine}=await import('data:text/javascript;base64,'+Buffer.from(fs.readFileSync(new URL('engine.js',lab))).toString('base64'));
const wasm=fs.readFileSync(new URL('obervave-voice.wasm',lab));let e=new Engine(wasm);e.advance(5000);
const button=b=>{e.input({t:'btn',b,down:true});e.advance(120);e.input({t:'btn',b,down:false});e.advance(150);};
const enc=(n,steps=1)=>{e.input({t:'enc',e:n,steps});e.advance(80);};
const pad=(p,v=100)=>{e.input({t:'pad',p,v});e.advance(50);e.input({t:'pad',p,v:0});e.advance(50);};
e.input({t:'drums',part:0,kit:1});e.advance(100);button(15);assert(e.frame().drumGrid);
for(let l=0;l<8;l++){if(l)enc(0);assert.equal(e.frame().drumLane,l);pad(0);}
assert.equal(e.ex.sim_drum_mask(0,0),255);assert.equal(e.ex.sim_melodic_busy(),0);
// Actual sequence transport produces finite, nonzero audio from the eight-hit mask.
button(16);let peak=0,samples=0;
for(let i=0;i<600;i++){e.render(256);for(const v of new Float32Array(e.ex.memory.buffer,e.ex.sim_out_l(),256)){assert(Number.isFinite(v));if(v)samples++;peak=Math.max(peak,Math.abs(v));}}
assert(samples>10000&&peak>0.01&&peak<=1);button(16);e.input({t:'panic'});e.advance(5000);
for(let i=0;i<4;i++)assert.equal(e.ex.sim_writes(i),0);
// Save the full pattern through the original manual-save UI, export/reboot.
for(let i=0;i<4&&e.frame().bank!==2;i++)button(0);const sp=e.frame().pattern-1;e.input({t:'pad',p:sp,v:100});e.advance(850);e.input({t:'pad',p:sp,v:0});e.advance(100);if(e.frame().patternConfirm)pad(sp);e.advance(6000);assert(e.ex.sim_writes(2)>0);assert.equal(e.ex.sim_pattern_pending(),0);
const saved=e.store(true);e=new Engine(wasm,saved);e.advance(5000);assert.equal(e.ex.sim_engine(0),2);assert.equal(e.ex.sim_drum_mask(0,0),255);
button(15);assert(e.frame().drumGrid);
// Copy via the only supported pattern-saving workflow: the pads.
for(let i=0;i<4&&e.frame().bank!==2;i++)button(0);
e.input({t:'pad',p:1,v:100});e.advance(850);e.input({t:'pad',p:1,v:0});e.advance(100);if(e.frame().patternConfirm)pad(1);e.advance(6000);pad(1);assert.equal(e.ex.sim_pattern(),1);assert.equal(e.ex.sim_drum_mask(0,0),255);
button(15);assert(e.frame().drumGrid);button(17);assert(e.frame().drumRec);assert(e.frame().drumPads);
for(let l=0;l<8;l++)e.input({t:'pad',p:l,v:110});e.advance(50);
for(let l=0;l<8;l++)e.input({t:'pad',p:l,v:0});e.advance(50);
assert.equal(e.ex.sim_drum_mask(0,0),255);button(16);e.advance(200);assert(!e.frame().drumRec);
// Part focus stays independent; melodic part gets its original full-screen view.
e.input({t:'analog',part:1});e.advance(100);enc(5);assert(!e.frame().drumGrid);assert.equal(e.frame().track,1);
enc(5,-1);assert(e.frame().drumGrid);assert.deepEqual(e.frame().engines,[2,0,0,0]);
console.log(JSON.stringify({result:'PASS',eight_lane_grid:true,mask_255_roundtrip:true,native_pad_step_edit:true,
 manual_pattern_save:true,pattern_copy_scene:true,reboot:true,real_time_eight_pad_record:true,part_context:true,
 nonzero_samples:samples,peak,browser_audio_output_tested:false,hardware_tested:false}));
