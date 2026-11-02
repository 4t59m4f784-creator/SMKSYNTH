import fs from 'node:fs';import assert from 'node:assert/strict';
const root=new URL('../web/voice-lab/',import.meta.url);const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',root)).toString('base64'));const wasm=fs.readFileSync(new URL('obervave-voice.wasm',root));let e=new Engine(wasm);e.advance(5000);
const b=(n,ms=100)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(150);};const pad=(p,ms=100)=>{e.input({t:'pad',p,v:100});e.advance(ms);e.input({t:'pad',p,v:0});e.advance(150);};
for(const bank of [0,3,2]){while(e.frame().bank!==bank)b(0);b(16);assert.equal(e.frame().bank,bank);b(17);assert.equal(e.frame().bank,bank);b(16);b(12,900);}
b(0,900);b(4);b(4);assert.equal(e.ex.sim_part_octave(0),-2);pad(1);b(5);assert.equal(e.ex.sim_part_octave(1),1);pad(0);assert.equal(e.ex.sim_part_octave(0),-2);
b(11);e.advance(6500);pad(8,850);e.advance(6500);b(0);b(0);assert.equal(e.frame().bank,2);const slot=e.frame().pattern-1;pad(slot,850);if(e.frame().patternConfirm)pad(slot);e.advance(6500);
e=new Engine(wasm,e.store(true));e.advance(5000);assert.equal(e.ex.sim_part_octave(0),-2);assert.equal(e.ex.sim_part_octave(1),1);
b(15);assert.equal(e.frame().bank,1);
console.log('PASS: PLAY/REC retain all three manual pad banks; SEQ alone takes step pads; per-part OCT physical controls, preset/favorite save, scene reboot retains A-2/B+1.');
