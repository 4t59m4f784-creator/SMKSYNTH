// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';
import assert from 'node:assert/strict';
const lab=new URL('../web/voice-lab/',import.meta.url);
const url='data:text/javascript;base64,'+Buffer.from(fs.readFileSync(new URL('engine.js',lab))).toString('base64');
const {Engine}=await import(url);const bytes=fs.readFileSync(new URL('obervave-voice.wasm',lab));
let e=new Engine(bytes);e.advance(5000);let samples=0,peak=0;
// Actual front-panel configuration, not only preset test shortcuts.
for(let i=0;i<4;i++){e.input({t:'btn',b:10,down:true});e.advance(150);e.input({t:'btn',b:10,down:false});e.advance(150);}
for(let i=0;i<2;i++){e.input({t:'enc',e:0,steps:1});e.advance(150);}
assert.deepEqual(e.frame().engines,[2,0,0,0]);
e.input({t:'btn',b:12,down:true});e.advance(100);e.input({t:'btn',b:12,down:false});e.advance(100);
for(let kit=0;kit<5;kit++){
 e.input({t:'drums',part:0,kit});e.advance(50);
 assert.equal(e.ex.sim_engine(0),2);
 for(let lane=0;lane<8;lane++){
  e.input({t:'panic'});e.advance(20);
  e.input({t:'drum-hit',part:0,lane,v:110});let energy=0;
  for(let b=0;b<100;b++){
   e.render(256);const a=new Float32Array(e.ex.memory.buffer,e.ex.sim_out_l(),256);
   for(const v of a){assert(Number.isFinite(v));energy+=v*v;if(v)samples++;peak=Math.max(peak,Math.abs(v));}
  }
  assert(energy>0.001,`silent kit ${kit}, lane ${lane}`);assert.equal(e.ex.sim_melodic_busy(),0);
 }
}
for(let i=0;i<4;i++)assert.equal(e.ex.sim_writes(i),0);
e.input({t:'analog',part:1});e.input({t:'drums',part:3,kit:1});e.advance(100);
assert.deepEqual(e.frame().engines,[2,0,0,2]);
e.input({t:'panic'});e.advance(5000);
e.input({t:'pad',p:8,v:100});e.advance(800);e.input({t:'pad',p:8,v:0});e.advance(5000);
for(let i=0;i<20&&e.ex.sim_pending();i++)e.advance(1000);
assert.equal(e.ex.sim_pending(),0);assert(e.ex.sim_writes(3)>0);
e=new Engine(bytes,e.store(true));e.advance(5000);
e.input({t:'pad',p:8,v:100});e.advance(50);e.input({t:'pad',p:8,v:0});e.advance(100);
assert.equal(e.ex.sim_engine_patch(0),2);assert.equal(e.ex.sim_engine(0),2);
// Sound edit pages preserve pads; only explicit KNOB BANK enters DRUM PADS.
for(let n=0;n<2;n++){e.input({t:'btn',b:13,down:true});e.advance(80);e.input({t:'btn',b:13,down:false});e.advance(80);}
assert.equal(e.ex.sim_drums_pad_mode(),0);
for(let n=0;n<3;n++){e.input({t:'btn',b:0,down:true});e.advance(80);e.input({t:'btn',b:0,down:false});e.advance(80);}
assert.equal(e.ex.sim_drums_pad_mode(),1);assert.equal(e.frame().bank,4);
e.input({t:'btn',b:0,down:true});e.advance(80);e.input({t:'btn',b:0,down:false});e.advance(80);assert.equal(e.ex.sim_drums_pad_mode(),0);
let Processor;
globalThis.AudioWorkletProcessor=class{constructor(){this.port={postMessage(){},onmessage:null};}};
globalThis.registerProcessor=(_name,ctor)=>{Processor=ctor;};globalThis.currentTime=0;
const worklet=fs.readFileSync(new URL('worklet.js',lab),'utf8').replace("'./engine.js'",JSON.stringify(url));
await import('data:text/javascript;base64,'+Buffer.from(worklet).toString('base64'));
const p=new Processor({processorOptions:{wasm:bytes}});p.engine.advance(5000);
p.port.onmessage({data:{t:'drums',part:0,kit:1}});p.engine.advance(100);
p.port.onmessage({data:{t:'drum-hit',part:0,lane:0,v:120}});let workletPeak=0;
for(let b=0;b<500;b++){globalThis.currentTime=b*128/44100;const out=[[new Float32Array(128),new Float32Array(128)]];assert(p.process([],out));for(const ch of out[0])for(const v of ch){assert(Number.isFinite(v));workletPeak=Math.max(workletPeak,Math.abs(v));}}
assert(workletPeak>0.01&&workletPeak<=1);
console.log(JSON.stringify({result:'PASS',kits:5,instruments:8,nonzero_samples:samples,peak,worklet_peak:workletPeak,
 melodic_cards_used_by_drums:0,favourite_roundtrip:true,explicit_drum_pads:true,browser_audio_output_tested:false,hardware_tested:false}));
