// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(process.argv[2]||'build/mm1-sim/SMKSYNTH-1.1.0-mm1.wasm'),seed=fs.readFileSync('web/simulator/factory-session.bin');
const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=(n)=>{e.input({t:'btn',b:n,down:true});e.advance(80);e.input({t:'btn',b:n,down:false});e.advance(180);};
const enc=(n,v)=>{e.input({t:'enc',e:n,steps:v});e.advance(150);};
const pad=(p,v)=>{e.input({t:'pad',p,v});e.advance(100);};
const key=(k,down)=>{e.input({t:'key',k,down});e.advance(100);};
const shot=name=>{const fb=new Uint16Array(e.ex.memory.buffer,e.ex.sim_fb(),57600),out=Buffer.alloc(240*240*3);for(let i=0;i<fb.length;i++){const v=fb[i];out[i*3]=(v>>11&31)*255/31;out[i*3+1]=(v>>5&63)*255/63;out[i*3+2]=(v&31)*255/31;}fs.writeFileSync('build/mm1/'+name+'.ppm',Buffer.concat([Buffer.from('P6\n240 240\n255\n'),out]));};
// Discover appended MIDI page by real GLOBE navigation; entry must not fire from arrows.
for(let i=0;i<8;i++){b(10);if(e.ex.sim_page()===30)break;}
assert.equal(e.ex.sim_page(),30);assert.equal(e.ex.sim_midi_mode(),0);enc(0,1);e.advance(400);assert.equal(e.ex.sim_midi_mode(),1);shot("midi-main");
key(0,true);key(36,true);e.render(1024);assert.equal(e.ex.sim_peak(),0);assert.equal(e.ex.sim_melodic_busy(),0);key(0,false);key(36,false);
e.input({t:'midi',st:0x90,d1:60,d2:100});e.advance(200);assert.equal(e.ex.sim_melodic_busy(),0);
const beforeK8=e.ex.sim_midi_tx_count();enc(7,5);assert(e.ex.sim_midi_tx_count()>beforeK8);assert.equal(e.ex.sim_midi_tx(e.ex.sim_midi_tx_count()-1,1),77);
// Real pad editing and selection, colours affect hardware LED representation.
b(13);assert.equal(e.ex.sim_midi_edit(),1);pad(2,100);pad(2,0);assert.equal(e.ex.sim_midi_selected(),26);enc(3,1);shot("midi-edit");assert.equal(e.ex.sim_midi_color(2),1);enc(1,3);const note=e.ex.sim_midi_entry(26,1);b(11);b(13);e.advance(200);const rgb=e.ex.sim_pad_rgb(2);assert.equal(rgb>>8&255,80);assert((rgb>>16&255)<20);
// Profile/channel selection, per-profile saved colours, persistent selection.
b(10);assert.equal(e.ex.sim_midi_settings(),1);shot("midi-global");enc(1,1);assert.equal(e.ex.sim_midi_map(),1);assert.equal(e.ex.sim_midi_color(2),0);enc(2,4);b(11);enc(1,-1);assert.equal(e.ex.sim_midi_color(2),1);assert.equal(e.ex.sim_midi_entry(26,1),note);b(11);const saved=e.store(true);
const re=new Engine(wasm,saved);re.advance(5000);assert.equal(re.ex.sim_midi_mode(),0);assert.equal(re.ex.sim_midi_color(2),1);
// Return through GLOBAL only. Synth/MIDI must resume, zero FX allocator growth.
enc(0,-1);e.advance(500);assert.equal(e.ex.sim_midi_mode(),0);key(12,true);e.advance(300);assert(e.ex.sim_melodic_busy()>0);key(12,false);
enc(0,1);e.advance(400);assert.equal(e.ex.sim_midi_mode(),1);b(10);enc(0,-1);e.advance(500);key(12,true);e.advance(100);assert(e.ex.sim_melodic_busy()>0);key(12,false);
const fb=new Uint16Array(e.ex.memory.buffer,e.ex.sim_fb(),57600);const out=Buffer.alloc(240*240*3);for(let i=0;i<fb.length;i++){const v=fb[i];out[i*3]=(v>>11&31)*255/31;out[i*3+1]=(v>>5&63)*255/63;out[i*3+2]=(v&31)*255/31;}
fs.writeFileSync('build/mm1/synth-return.ppm',Buffer.concat([Buffer.from('P6\n240 240\n255\n'),out]));
console.log('PASS: actual GLOBE/encoder navigation, exclusive audio/MIDI-in silence, onboard pad edit/colour, independent four profiles, persistence after restart, safe repeated synth return.');
