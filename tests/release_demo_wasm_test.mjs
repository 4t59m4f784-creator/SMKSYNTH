import fs from 'node:fs';import assert from 'node:assert/strict';
const lab=new URL('../web/simulator/',import.meta.url);const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',lab)).toString('base64'));
const wasm=fs.readFileSync(new URL('obervave-voice.wasm',lab)),seed=fs.readFileSync(new URL('factory-session.bin',lab));
let e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=(n,ms=100)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(130);};
const pad=p=>{e.input({t:'pad',p,v:100});e.advance(80);e.input({t:'pad',p,v:0});e.advance(160);};
assert.equal(e.frame().patternsUsed,31);assert.equal(e.frame().used,255);assert.equal(e.ex.sim_tempo(),90);
assert.equal(e.frame().pattern,1);assert.equal(e.frame().bank,0);assert.deepEqual(Array.from(seed.subarray(4096,4096+8192)),Array(8192).fill(0));
const energies=[];
for(let slot=0;slot<5;slot++){
 while(e.frame().bank!==2)b(0);pad(slot);assert.equal(e.frame().pattern,slot+1);b(16);
 let energy=0;for(let i=0;i<300;i++){e.render(256);const p=e.ex.sim_out_l();for(const x of new Float32Array(e.ex.memory.buffer,p,256)){assert(Number.isFinite(x));energy+=x*x;}}
 assert(energy>0.01,'example '+(slot+1)+' must sound');energies.push(energy);b(16);
}
// Favourites are standalone snapshots, including custom sounds with no preset writes.
while(e.frame().bank!==0)b(0);for(let i=0;i<8;i++){pad(i+8);assert.equal(e.frame().favourite,i+1);e.input({t:'key',k:12,down:true});e.advance(250);e.input({t:'key',k:12,down:false});e.advance(500);}
for(let t=0;t<4;t++)assert.equal(e.ex.sim_writes(t),0);
e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);assert.equal(e.frame().patternsUsed,31);assert.equal(e.frame().used,255);
console.log(JSON.stringify({result:'PASS',real_firmware_boot:true,patterns:5,free_slots:11,favorites:8,extra_presets:0,tempo:90,pattern_audio_energy:energies,all_favorite_recalls:true,no_auto_writes:true,hardware_tested:false}));
