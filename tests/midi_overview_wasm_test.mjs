// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(process.argv[2]),seed=fs.readFileSync('web/simulator/factory-session.bin');const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=(n,ms=80)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(150);};const enc=(n,v)=>{e.input({t:'enc',e:n,steps:v});e.advance(150);};const key=(k,down)=>{e.input({t:'key',k,down});e.advance(100);};const pad=(p,v)=>{e.input({t:'pad',p,v});e.advance(100);};
const shot=n=>{const fb=new Uint16Array(e.ex.memory.buffer,e.ex.sim_fb(),57600),o=Buffer.alloc(172800);for(let i=0;i<57600;i++){let v=fb[i];o[i*3]=(v>>11&31)*255/31;o[i*3+1]=(v>>5&63)*255/63;o[i*3+2]=(v&31)*255/31;}fs.writeFileSync('build/mo1/'+n+'.ppm',Buffer.concat([Buffer.from('P6\n240 240\n255\n'),o]));};
for(let i=0;i<9&&e.ex.sim_page()!==30;i++)b(10);assert.equal(e.ex.sim_page(),30);enc(0,1);assert.equal(e.ex.sim_midi_mode(),1);assert.equal(e.ex.sim_midi_ui_control(),71);shot('wasm-idle');
for(let i=0;i<8;i++)enc(i,i+2);for(let i=0;i<4;i++){e.input({t:'fader',f:i,v:35+i*20});e.advance(120);}enc(2,4);const ctrl=e.ex.sim_midi_ui_control(),val=e.ex.sim_midi_ui_value();assert.equal(ctrl,2);
pad(8,113);key(12,true);key(16,true);key(19,true);assert.equal(e.ex.sim_midi_ui_control(),ctrl);assert.equal(e.ex.sim_midi_ui_value(),val);assert.equal(e.ex.sim_midi_ui_pad(),32);assert.equal(e.ex.sim_midi_ui_pad_value(),113);for(const [k,n]of [[12,60],[16,64],[19,67]])assert.equal(e.ex.sim_midi_ui_note(k),n);shot('wasm-chord');
const pix=(x,y)=>new Uint16Array(e.ex.memory.buffer,e.ex.sim_fb(),57600)[y*240+x];assert.equal(pix(81,230),0xF964); // C_RED in RGB565
b(5); // OCT up while notes remain held
assert.equal(e.ex.sim_midi_ui_note(12),60);key(14,true);assert.equal(e.ex.sim_midi_ui_note(14),74);shot('wasm-held-octave');
for(const k of [12,14,16,19])key(k,false);pad(8,0);assert.equal(e.ex.sim_midi_ui_control(),ctrl);assert.equal(e.ex.sim_midi_ui_value(),val);assert.equal(e.ex.sim_midi_ui_pad_value(),113);assert.notEqual(pix(81,230),0xF964);shot('wasm-released');
b(1);e.input({t:'fader',f:1,v:38});e.advance(160);assert.equal(e.ex.sim_midi_ui_control(),21);assert.equal(e.ex.sim_midi_ui_value(),38);shot('wasm-fader');
// The panel renders physical notes and independent pad/controller feedback even under output backpressure.
e.ex.sim_midi_blocked(1);pad(8,105);key(7,true);enc(0,2);const last=e.ex.sim_midi_ui_control(),value=e.ex.sim_midi_ui_value();key(7,false);pad(8,0);assert.equal(e.ex.sim_midi_ui_control(),last);assert.equal(e.ex.sim_midi_ui_value(),value);assert.equal(e.ex.sim_midi_ui_pad_value(),105);e.ex.sim_midi_blocked(0);e.advance(300);
b(10);enc(0,-1);assert.equal(e.ex.sim_midi_mode(),0);key(12,true);assert(e.ex.sim_peak()>0);key(12,false);
console.log('PASS: real controls populate both bar groups; pads/chords never replace last controller; original held pitches survive OCT; key releases repaint; bank B fader, independent blocked-output updates, and local synth return.');
