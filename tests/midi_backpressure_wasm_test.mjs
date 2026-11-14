// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(process.argv[2]),seed=fs.readFileSync('web/simulator/factory-session.bin');
const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=n=>{e.input({t:'btn',b:n,down:true});e.advance(80);e.input({t:'btn',b:n,down:false});e.advance(160);};
const enc=(n,v)=>{e.input({t:'enc',e:n,steps:v});e.advance(160);};
const params=()=>Array.from({length:4},(_,t)=>Array.from({length:80},(_,p)=>e.ex.sim_param(t,p)));
for(let i=0;i<8&&e.ex.sim_page()!==30;i++)b(10);assert.equal(e.ex.sim_page(),30);const before=params();
for(let n=0;n<12;n++){
 e.ex.sim_midi_blocked(1);enc(0,1);assert.equal(e.ex.sim_midi_mode(),1);assert.equal(e.ex.sim_midi_panic(),80);
 enc(2,4);assert.equal(e.ex.sim_midi_last(),2);assert(e.ex.sim_midi_last_value()>0);
 e.input({t:'fader',f:0,v:93});e.advance(150);assert.equal(e.ex.sim_midi_last(),16);assert.equal(e.ex.sim_midi_last_value(),93);
 if(n%2){e.input({t:'btn',b:0,down:true});e.input({t:'btn',b:1,down:true});e.advance(80);e.input({t:'btn',b:0,down:false});e.input({t:'btn',b:1,down:false});e.advance(100);}
 const previous=e.ex.sim_fb_hash();e.input({t:'pad',p:8,v:113});e.advance(100);
 if(n%2){assert.equal(e.ex.sim_midi_key_channels(),256);assert.equal(e.ex.sim_midi_last(),16);}else{assert.equal(e.ex.sim_midi_last(),32);assert.equal(e.ex.sim_midi_last_value(),113);}
 assert.notEqual(e.ex.sim_fb_hash(),previous);e.input({t:'pad',p:8,v:0});e.advance(100);
 if(n%2){assert.equal(e.ex.sim_midi_key_channels(),256);assert.equal(e.ex.sim_midi_channel_held(),0);}else assert.equal(e.ex.sim_midi_last_value(),113);
 // Exit through the real controller GLOBAL, with the host still blocked.
 b(10);enc(0,-1);assert.equal(e.ex.sim_midi_mode(),0);assert.equal(e.ex.sim_midi_return_wait(),0);assert.equal(e.ex.sim_page(),30);
 const fb=new Uint16Array(e.ex.memory.buffer,e.ex.sim_fb(),57600);assert(new Set(fb).size>3,'Synth MIDI entry page must render instead of going black');
 e.input({t:'key',k:12,down:true});e.advance(300);assert(e.ex.sim_melodic_busy()>0);assert(e.ex.sim_peak()>0);e.input({t:'key',k:12,down:false});e.advance(200);
 e.ex.sim_midi_blocked(0);e.advance(600);assert.equal(e.ex.sim_midi_panic(),0);assert.deepEqual(params(),before);
 e.input({t:'fader',f:0,v:0});e.advance(100);
}
console.log('PASS: 12 real blocked-host MIDI/synth round trips; encoders/faders repaint under backpressure, visible synth return, local audio alive, safety flush resumes, all four patches unchanged.');
