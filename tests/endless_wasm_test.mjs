// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(process.argv[2]||'build/endless-e4/SMKSYNTH-1.0.3-e4.wasm'),seed=fs.readFileSync('web/simulator/factory-session.bin');
const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=(n,ms=80)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(180);};
const pad=(p,ms=100)=>{e.input({t:'pad',p,v:100});e.advance(ms);e.input({t:'pad',p,v:0});e.advance(250);};
const key=(k,on)=>{e.input({t:'key',k,down:on});e.advance(60);for(let i=0;on&&e.ex.sim_endless_pending()&&i<300;i++)e.advance(1);assert(!on||!e.ex.sim_endless_pending());};
const enc=(knob,steps)=>{e.input({t:'enc',e:knob,steps});e.advance(100);};
const page=()=>{b(12,900);for(let i=0;i<9;i++){b(15);if(e.ex.sim_seq_view()===6)return;}assert.fail('ENDLESS missing');};
b(0);b(0);pad(5);assert.equal(e.frame().pattern,6);page();
assert.equal(e.ex.sim_endless_mode(),1);for(let t=0;t<4;t++)assert.equal(e.ex.sim_endless_enabled(t),1);
// Per-part append recording is unchanged. Global activation never erases another phrase.
for(const [t,notes]of [[0,[12,16,19]],[1,[0,4,7,12,16]],[2,[0,2,4,5,7,9,11]]]){
 if(t)enc(5,1);assert.equal(e.frame().track,t);assert.equal(e.ex.sim_seq_view(),6);b(17);assert.equal(e.ex.sim_endless_record(),t+1);assert.equal(e.ex.sim_seq_length(t),0);
 for(const k of notes){key(k,true);key(k,false);}b(17);assert.equal(e.ex.sim_seq_length(t),notes.length);
}
assert.deepEqual([0,1,2].map(t=>e.ex.sim_seq_length(t)),[3,5,7]);
// DRUM-X retains its editor/steps. Its sequence joins the same transport.
e.input({t:'drums',part:3,kit:0});e.advance(100);enc(5,1);assert.equal(e.frame().track,3);assert.notEqual(e.ex.sim_seq_view(),6);pad(0);assert(e.ex.sim_drum_step_bits()&1);
enc(5,-1);assert.equal(e.frame().track,2);assert.equal(e.ex.sim_seq_view(),6);assert.equal(e.ex.sim_endless_mode(),1);
const pat=e.frame().pattern;const tempo=e.ex.sim_tempo();enc(4,120-tempo);assert.equal(e.ex.sim_tempo(),120);assert.equal(e.frame().pattern,pat);assert.deepEqual([0,1,2].map(t=>e.ex.sim_seq_length(t)),[3,5,7]);
// Every key-on, including legato, retriggers all first steps. Fixed drum pitch.
key(0,true);e.advance(200);key(2,true);for(let t=0;t<4;t++)assert.equal(e.ex.sim_seq_pos(t),0);
assert.deepEqual([0,1,2,3].map(t=>e.ex.sim_endless_transpose(t)),[2,2,2,0]);
let prev=e.ex.sim_seq_pos(0),wrapped=false;
for(let i=0;i<600&&!wrapped;i++){e.advance(1);const pos=e.ex.sim_seq_pos(0);if(prev===2&&pos===0){wrapped=true;assert.deepEqual([1,2,3].map(t=>e.ex.sim_seq_pos(t)),[3,3,3]);}prev=pos;}assert(wrapped);
enc(3,-1);assert.equal(e.ex.sim_endless_hold(),0);key(0,false);assert.equal(e.ex.sim_transport(),1);key(2,false);assert.equal(e.ex.sim_transport(),0);assert.equal((e.frame().leds>>>16)&1,0);for(let t=0;t<4;t++)assert.equal(e.ex.sim_seq_pos(t),0);
// Explicit PLAY and the next note restart; HOLD ON survives key release.
b(16);assert.equal(e.ex.sim_transport(),1);key(5,true);for(let t=0;t<4;t++)assert.equal(e.ex.sim_seq_pos(t),0);key(5,false);assert.equal(e.ex.sim_transport(),0);
enc(3,1);key(0,true);key(0,false);assert.equal(e.ex.sim_transport(),1);e.advance(1000);b(16);assert.equal(e.ex.sim_transport(),0);
// Real-loop scheduled target: the old harmony stays until its clock boundary.
key(0,true);e.advance(30);const old=e.ex.sim_endless_transpose(0);e.input({t:'key',k:2,down:true});e.advance(1);assert.equal(e.ex.sim_endless_pending(),1);assert.equal(e.ex.sim_endless_transpose(0),old);e.advance(130);assert.equal(e.ex.sim_endless_pending(),0);assert.equal(e.ex.sim_endless_transpose(0),2);key(0,false);key(2,false);b(16);
// No idle redraws after transient messages, plus actual saved-pattern recall.
e.advance(2500);const traffic=()=>[e.ex.sim_lcd_fills(),e.ex.sim_lcd_blits(),e.ex.sim_lcd_pixels()];const before=traffic();e.advance(1200);assert.deepEqual(traffic(),before);
b(0);b(0);b(0);assert.equal(e.frame().bank,2);pad(5,850);e.advance(8000);assert(e.frame().patternsUsed&(1<<5));pad(0);assert.equal(e.ex.sim_endless_mode(),0);pad(5);assert.equal(e.frame().pattern,6);assert.equal(e.ex.sim_endless_mode(),1);assert.equal(e.ex.sim_endless_hold(),1);assert.deepEqual([0,1,2].map(t=>e.ex.sim_seq_length(t)),[3,5,7]);
// Physical OCT is recorded, but never offsets the ENDLESS launch root.
b(12,900);b(5);assert.equal(e.ex.sim_part_octave(2),1);b(15);assert.equal(e.ex.sim_seq_view(),6);
const append=e.ex.sim_seq_length(2);b(17);key(0,true);key(0,false);b(17);assert.equal(e.ex.sim_step_note(2,append,0),60);
key(0,true);assert.equal(e.ex.sim_endless_transpose(0),0);key(0,false);b(16);
enc(5,-1);b(12,900);b(4);assert.equal(e.ex.sim_part_octave(1),-1);b(15);key(0,true);assert.equal(e.ex.sim_endless_transpose(0),0);key(0,false);b(16);
console.log(JSON.stringify({result:'PASS',real_firmware_loop:true,all_parts_default:true,independent_lengths:[3,5,7],global_legato_retrigger:true,soft_q16_scheduled_harmony:true,recorded_part_octave:true,fixed_c3_across_parts:true,global_hold_and_play_LED:true,drums_fixed_and_synced:true,analog_drum_focus_persistence:true,k5_tempo:120,idle_redraws:0,pattern6_saved_and_recalled:true}));
