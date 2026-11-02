// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';
import assert from 'node:assert/strict';
const lab=new URL('../web/voice-lab/',import.meta.url);
const code=fs.readFileSync(new URL('engine.js',lab),'utf8');
const engineUrl='data:text/javascript;base64,'+Buffer.from(code).toString('base64');
const {Engine}=await import(engineUrl);
const bytes=fs.readFileSync(new URL('obervave-voice.wasm',lab));
assert.equal(WebAssembly.Module.imports(new WebAssembly.Module(bytes)).length,0);
let e=new Engine(bytes);e.advance(5000);
assert.deepEqual(e.frame().engines,[0,0,0,0]);
e.input({t:'voice',part:0,preset:0});e.advance(30);assert.equal(e.ex.sim_engine(0),1);
e.input({t:'key',k:12,down:true});
let nonzero=0,peak=0;
for(let b=0;b<400;b++){
 e.render(256);let a=new Float32Array(e.ex.memory.buffer,e.ex.sim_out_l(),256);
 for(const v of a){assert(Number.isFinite(v));if(v)nonzero++;peak=Math.max(peak,Math.abs(v));}
}
assert(nonzero>50000&&peak>0.01&&peak<=1);
e.input({t:'key',k:12,down:false});e.advance(4000);
for(let i=0;i<4;i++)assert.equal(e.ex.sim_writes(i),0);
// Explicit favourite save and restore, in the actual WASM fake flash.
e.input({t:'pad',p:8,v:100});e.advance(800);e.input({t:'pad',p:8,v:0});e.advance(5000);
for(let i=0;i<20&&e.ex.sim_pending();i++)e.advance(1000);
assert.equal(e.ex.sim_pending(),0);assert(e.ex.sim_writes(3)>0);
const store=e.store(true);assert.equal(e.ex.sim_favourites_used()&1,1);
e=new Engine(bytes,store);e.advance(5000);assert.equal(e.ex.sim_favourites_used()&1,1);
e.input({t:'pad',p:8,v:100});e.advance(40);e.input({t:'pad',p:8,v:0});e.advance(100);
assert.equal(e.ex.sim_engine(0),1);assert.equal(e.ex.sim_engine_patch(0),1);
// Mixed per-track engine choices are retained independently.
e.input({t:'voice',part:1,preset:1});e.input({t:'voice',part:3,preset:3});e.advance(100);
assert.deepEqual(e.frame().engines,[1,1,0,1]);
e.input({t:'analog',part:0});e.advance(100);assert.deepEqual(e.frame().engines,[0,1,0,1]);

// Run the real worklet process() with the standard audio-worklet host surface.
let Processor;
globalThis.AudioWorkletProcessor=class{constructor(){this.port={postMessage(){},onmessage:null};}};
globalThis.registerProcessor=(_name,ctor)=>{Processor=ctor;};globalThis.currentTime=0;
const workletCode=fs.readFileSync(new URL('worklet.js',lab),'utf8').replace("'./engine.js'",JSON.stringify(engineUrl));
await import('data:text/javascript;base64,'+Buffer.from(workletCode).toString('base64'));
const p=new Processor({processorOptions:{wasm:bytes}});p.engine.advance(5000);
p.port.onmessage({data:{t:'voice',part:0,preset:2}});p.engine.advance(50);
p.port.onmessage({data:{t:'key',k:12,down:true}});
let workletPeak=0;
for(let n=0;n<800;n++){globalThis.currentTime=n*128/44100;
 let out=[[new Float32Array(128),new Float32Array(128)]];assert(p.process([],out));
 for(let c=0;c<2;c++)for(const v of out[0][c]){assert(Number.isFinite(v));workletPeak=Math.max(workletPeak,Math.abs(v));}}
assert(workletPeak>0.01&&workletPeak<=1);
console.log(JSON.stringify({result:'PASS',wasm_boot:true,native_engine_routing:true,audible_nonzero_samples:nonzero,peak,
 favourite_roundtrip:true,independent_tracks:true,worklet_process:true,worklet_peak:workletPeak,
 browser_audio_output_tested:false,hardware_tested:false}));
