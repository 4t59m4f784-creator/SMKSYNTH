// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const e=new Engine(fs.readFileSync(process.argv[2]));e.advance(5000);
const b=n=>{e.input({t:'btn',b:n,down:true});e.advance(60);e.input({t:'btn',b:n,down:false});e.advance(120);};
const turn=(k,s)=>{e.input({t:'enc',e:k,steps:s});e.advance(120);};
const patches=()=>Array.from({length:4},(_,t)=>Array.from({length:80},(_,p)=>e.ex.sim_param(t,p)));
for(let n=0;n<8&&e.ex.sim_page()!==30;n++)b(10);assert.equal(e.ex.sim_page(),30);
const selected=e.ex.sim_midi_map();for(let k=1;k<4;k++)turn(k,10);assert.equal(e.ex.sim_midi_map(),selected);assert.equal(e.ex.sim_midi_mode(),0);
const before=patches();
for(let n=0;n<5;n++){
 turn(0,1);assert.equal(e.ex.sim_midi_mode(),1);e.input({t:'midi',st:0xB0,d1:1,d2:127});e.input({t:'midi',st:0xE0,d1:127,d2:127});e.advance(100);
 e.input({t:'wheel',w:1,v:485});e.advance(100);e.input({t:'wheel',w:1,v:620});e.advance(100);b(10);turn(0,-1);e.advance(500);
 assert.equal(e.ex.sim_midi_mode(),0);assert.equal(e.ex.sim_synth_vibrato(),0);assert.equal(e.ex.sim_synth_bend(),0);assert.deepEqual(patches(),before);
}
turn(0,1);e.advance(200);b(13);e.advance(1000);const count=e.ex.sim_lcd_pixels();e.advance(1000);assert.equal(e.ex.sim_lcd_pixels(),count); // editor LEDs blink independently from LCD
console.log('PASS: real loop entry disables K2-K4, five mode round trips preserve all patch bytes and clear mod/bend with released wheels; editor LCD stays idle, LED blinking remains separate.');
