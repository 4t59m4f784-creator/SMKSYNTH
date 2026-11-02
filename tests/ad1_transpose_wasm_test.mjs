// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';import fs from 'node:fs';
const lab=new URL('../web/voice-lab/',import.meta.url);
const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',lab)).toString('base64'));
const wasm=fs.readFileSync(new URL('obervave-voice.wasm',lab));const e=new Engine(wasm);e.advance(5000);
const button=b=>{e.input({t:'btn',b,down:true});e.advance(120);e.input({t:'btn',b,down:false});e.advance(150);};
const enc=(n,steps=1)=>{e.input({t:'enc',e:n,steps});e.advance(100);};
const key=(k,on)=>{e.input({t:'key',k,down:on});e.advance(80);};
assert.equal(e.ex.sim_voice_preset,undefined);assert.equal(e.ex.sim_morph,undefined);
// Navigate the real panel, rather than set internal sequencer state.
for(let i=0;i<5;i++)button(15);assert.equal(e.ex.sim_seq_view(),5);
enc(0);assert.equal(e.ex.sim_transpose_keys(),1);
key(0,true);key(0,false);assert.equal(e.ex.sim_seq_transpose(),0);
key(2,true);key(2,false);assert.equal(e.ex.sim_seq_transpose(),2);assert.equal(e.ex.sim_melodic_busy(),0);
button(4);key(11,true);key(11,false);assert.equal(e.ex.sim_seq_transpose(),-1); // B2
button(5);enc(5);assert.equal(e.ex.sim_seq_transpose(),-1);assert.equal(e.frame().track,1);
enc(2);assert.equal(e.ex.sim_seq_transpose(),0);enc(1);assert.equal(e.ex.sim_seq_transpose(),1);
enc(0,-1);assert.equal(e.ex.sim_transpose_keys(),0);assert.equal(e.ex.sim_seq_transpose(),1);
enc(0);key(7,true);button(12);assert.equal(e.ex.sim_transpose_keys(),1);const latched=e.ex.sim_seq_transpose();key(7,false);assert.equal(e.ex.sim_seq_transpose(),latched);assert.equal(e.ex.sim_melodic_busy(),0);
enc(3,-1);assert.equal(e.ex.sim_transpose_keys(),0);
// DRUM-X context has the same page, without replacing its drum grid.
e.input({t:'drums',part:1,kit:1});e.advance(200);button(15);assert(e.frame().drumGrid);
for(let i=0;i<5;i++)button(15);assert.equal(e.ex.sim_seq_view(),5);enc(0);key(2,true);key(2,false);assert.equal(e.ex.sim_seq_transpose(),2);assert.equal(e.ex.sim_melodic_busy(),0);
enc(4);assert.equal(e.ex.sim_seq_transpose(),2); // stopped pattern navigation
for(let t=0;t<4;t++)assert.equal(e.ex.sim_writes(t),0);
console.log(JSON.stringify({result:'PASS',panel_navigation:true,C3:0,D3:2,B2:-1,reset:true,latch:true,capture_noteoffs:true,no_auto_writes:true,retired_voice_unavailable:true,hardware_tested:false}));
