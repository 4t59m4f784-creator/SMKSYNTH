import fs from 'node:fs';import assert from 'node:assert/strict';
const lab=new URL('../web/voice-lab/',import.meta.url);const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',lab)).toString('base64'));const wasm=fs.readFileSync(new URL('obervave-voice.wasm',lab));const e=new Engine(wasm);e.advance(5000);
const b=(n,ms=120)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(200);};const pad=n=>{e.input({t:'pad',p:n,v:100});e.advance(100);e.input({t:'pad',p:n,v:0});e.advance(100);};
assert.equal(e.frame().bank,0);const program=e.frame().programs[0];pad(4);pad(5);assert.equal(e.frame().programs[0],program);pad(7);assert.equal(e.frame().programs[0],program+1);pad(6);assert.equal(e.frame().programs[0],program);
for(let i=0;i<12;i++){b(0);assert.equal(e.frame().bank,[3,2,0][i%3]);}
b(0);b(0,900);assert.equal(e.frame().bank,0);assert.equal(e.ex.sim_page(),0);
// Short PATCH still reaches mixer; long PATCH always returns performance, not its next row.
b(12);assert.notEqual(e.ex.sim_page(),0);for(let i=0;i<8;i++)b(12);b(12,900);assert.equal(e.ex.sim_page(),0);
for(const n of [6,10,13,14,15]){b(n);b(12,900);assert.equal(e.ex.sim_page(),0);}
// Each sequencer view exits cleanly to manual pads; no hidden manual bank advance.
for(let i=0;i<8;i++){b(15);assert.equal(e.frame().bank,1);b(0);assert.equal(e.frame().bank,0);assert.equal(e.ex.sim_page(),0);b(0);assert.equal(e.frame().bank,3);b(0,900);}
// PLAY/REC take ownership; manual bank access does not stop the transport/REC.
b(16);assert.equal(e.frame().bank,0);b(0);assert.equal(e.frame().bank,3);b(17);assert.equal(e.frame().bank,3);b(0,900);assert.equal(e.frame().bank,0);b(12,900);assert.equal(e.ex.sim_page(),0);b(16);
console.log('PASS: pad 5/6 FX, 7/8 presets; three-bank cycle; long bank diagnostic/home; long PATCH from mixer; repeated SEQ exits; PLAY/REC preserve pad ownership; real firmware main loop.');
