// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(process.argv[2]||'build/controls/SMKSYNTH-1.0.3.wasm'),seed=fs.readFileSync('web/simulator/factory-session.bin');
const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=(n,ms=80)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(180);};
const pad=(p,ms=100)=>{e.input({t:'pad',p,v:100});e.advance(ms);e.input({t:'pad',p,v:0});e.advance(250);};
const key=(k,on)=>{e.input({t:'key',k,down:on});e.advance(60);};
const page=()=>{b(12,900);for(let i=0;i<9;i++){b(15);if(e.ex.sim_seq_view()===6)return;}assert.fail('ENDLESS not in SEQ cycle');};
// Work on empty P6; no example pattern is overwritten.
b(0);b(0);assert.equal(e.frame().bank,2);pad(5);assert.equal(e.frame().pattern,6);page();
b(17);assert.equal(e.ex.sim_endless_record(),1);assert.equal(e.ex.sim_seq_length(0),0);
key(12,true);key(16,true);key(19,true);assert.equal(e.ex.sim_seq_length(0),1);key(12,false);key(16,false);key(19,false);
assert.deepEqual([0,1,2].map(i=>e.ex.sim_step_note(0,0,i)),[60,64,67]);
b(3);assert.equal(e.ex.sim_seq_length(0),2);assert.equal(e.ex.sim_step_note(0,1,0),0);
key(24,true);b(3);key(24,false);assert.equal(e.ex.sim_seq_length(0),4);assert.equal(e.ex.sim_step_gate(0,2),127);assert.equal(e.ex.sim_step_note(0,3,0),72);
b(17);assert.equal(e.ex.sim_endless_record(),0);assert.equal(e.ex.sim_transport(),0);
// HOLD OFF captures keyboard transposition and releases sequenced gates.
e.input({t:'enc',e:3,steps:-1});e.advance(150);key(2,true);assert.equal(e.ex.sim_endless_transpose(0),2);assert.equal(e.ex.sim_transport(),1);e.advance(200);key(2,false);e.advance(40);assert.equal(e.ex.sim_gates_part(0,1),0);b(16);assert.equal(e.ex.sim_transport(),0);
// Unchanged page generates no repeated traffic after toast expires.
e.advance(2500);let old=[e.ex.sim_lcd_fills(),e.ex.sim_lcd_blits(),e.ex.sim_lcd_pixels()];e.advance(1200);assert.deepEqual([e.ex.sim_lcd_fills(),e.ex.sim_lcd_blits(),e.ex.sim_lcd_pixels()],old);
// Save P6 through actual pattern-pad UI, switch away and recall the stored mode/phrase.
b(0);b(0);b(0);assert.equal(e.frame().bank,2);pad(5,850);e.advance(6000);assert(e.frame().patternsUsed&(1<<5));pad(0);assert.equal(e.frame().pattern,1);pad(5);assert.equal(e.frame().pattern,6);assert.equal(e.ex.sim_endless_enabled(0),1);assert.equal(e.ex.sim_seq_length(0),4);assert.equal(e.ex.sim_step_gate(0,2),127);
const bankWrites=e.frame().writes;
console.log(JSON.stringify({result:'PASS',real_firmware_loop:true,phrase:[['C4','E4','G4'],'REST','C5~','C5'],length:4,transport_and_hold:true,transpose:2,idle_redraws:0,pattern6_saved_and_recalled:true,writes:bankWrites}));
