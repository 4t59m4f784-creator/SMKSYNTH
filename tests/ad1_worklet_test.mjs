// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import vm from 'node:vm';
const lab=new URL('../web/voice-lab/',import.meta.url);
const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',lab)).toString('base64'));
const messages=[];let Worklet;
class Processor{constructor(){this.port={postMessage:m=>messages.push(m)}}}
const ctx={Engine,AudioWorkletProcessor:Processor,registerProcessor:(n,c)=>Worklet=c,currentTime:0,Float32Array};
vm.runInNewContext(fs.readFileSync(new URL('worklet.js',lab),'utf8').replace(/^import .*\n/m,''),ctx);
const p=new Worklet({processorOptions:{wasm:fs.readFileSync(new URL('obervave-voice.wasm',lab))}});
let energy=0,peak=0;
for(let b=0;b<3500;b++){
 ctx.currentTime=b*128/44100;
 if(b==1800)p.port.onmessage({data:{t:'key',k:14,down:true}});
 if(b==2100)p.port.onmessage({data:{t:'key',k:14,down:false}});
 if(b==2200)p.port.onmessage({data:{t:'drums',part:1,kit:0}});
 if(b==2300)p.port.onmessage({data:{t:'drum-hit',part:1,lane:0,v:100}});
 const out=[new Float32Array(128),new Float32Array(128)];assert(p.process([],[out]));
 for(const channel of out)for(const v of channel){assert(Number.isFinite(v)&&Math.abs(v)<=1);energy+=v*v;peak=Math.max(peak,Math.abs(v));}
}
assert(energy>1&&peak>0.01);assert(!messages.some(m=>m.t==='error'));assert(messages.some(m=>m.t==='ready'&&m.audio));assert(messages.some(m=>m.t==='frame'&&m.fb?.length===57600));
console.log(JSON.stringify({result:'PASS',actual_worklet_process:true,frames:3500*128,channels:2,energy,peak,errors:0,browser_output_tested:false,physical_tested:false}));
