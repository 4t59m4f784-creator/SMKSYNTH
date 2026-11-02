// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';
const lab=new URL('../web/voice-lab/',import.meta.url);const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',lab)).toString('base64'));const wasm=fs.readFileSync(new URL('obervave-voice.wasm',lab));let e=new Engine(wasm);e.advance(5000);
const button=b=>{e.input({t:'btn',b,down:true});e.advance(120);e.input({t:'btn',b,down:false});e.advance(180);};
const enc=(n,steps=1)=>{e.input({t:'enc',e:n,steps});e.advance(100);};
const pad=(p,v)=>{e.input({t:'pad',p,v});e.advance(100);};const key=(k,down)=>{e.input({t:'key',k,down});e.advance(100);};
assert.equal(e.frame().keyboardMode,0);button(6);for(let t=0;t<2;t++){if(t)enc(5);enc(0,-20);assert.equal(e.ex.sim_param(t,47),0);}enc(5,-1);button(12);
button(0);assert.equal(e.frame().bank,3);assert(e.ex.sim_keyboard_page());
pad(0,100);pad(1,100);pad(0,0);pad(1,0);assert.equal(e.frame().keyboardMode,2);assert.deepEqual(e.frame().keyboardParts,[0,1]);
key(0,true);assert(e.ex.sim_gates_part(0,0)>0&&e.ex.sim_gates_part(1,0)>0);key(0,false);e.advance(200);assert.equal(e.ex.sim_melodic_busy(),0);
// Main controls preserve the independent track mixer. Octave page is K5.
enc(1,-10);assert.equal(e.frame().keyboardLevels[0],70);button(0);button(0);assert.equal(e.frame().bank,0);button(0);assert.equal(e.frame().bank,3);assert.equal(e.frame().keyboardMode,2);
enc(4);enc(2);assert.deepEqual(e.frame().keyboardOctaves,[0,1]);
pad(5,100);pad(5,0);assert.equal(e.frame().keyboardMode,1);key(0,true);assert(e.ex.sim_gates_part(0,0)>0&&!e.ex.sim_gates_part(1,0));key(0,false);key(12,true);assert(!e.ex.sim_gates_part(0,0)&&e.ex.sim_gates_part(1,0));key(12,false);
pad(6,100);pad(6,0);assert.equal(e.frame().track,1);button(17);key(0,true);key(0,false);assert.equal(e.ex.sim_step_note(1,0,0),60);assert.equal(e.ex.sim_step_note(0,0,0),0);button(16);e.advance(1000);
for(let i=0;i<4&&e.frame().bank!==2;i++)button(0);const sp=e.frame().pattern-1;e.input({t:'pad',p:sp,v:100});e.advance(850);e.input({t:'pad',p:sp,v:0});e.advance(100);if(e.frame().patternConfirm){pad(sp,100);pad(sp,0);}e.advance(7000);assert(e.ex.sim_writes(2)>0);const saved=e.store(true);e=new Engine(wasm,saved);e.advance(5000);assert.equal(e.frame().keyboardMode,2);assert.deepEqual(e.frame().keyboardParts,[0,1]);assert.deepEqual(e.frame().keyboardOctaves,[0,1]);assert.deepEqual(e.frame().keyboardLevels,[70,100]);assert.equal(e.ex.sim_step_note(1,0,0),60);
button(0);pad(4,100);pad(4,0);assert.equal(e.frame().keyboardMode,0);
console.log(JSON.stringify({result:'PASS',bank4:true,two_pad_gesture:true,layer_two_parts:true,split_boundary:true,octaves:true,exclusive_keyboard_levels:true,selected_only_record:true,manual_pattern_save_and_reboot:true,single_restore:true,physical_tested:false}));
