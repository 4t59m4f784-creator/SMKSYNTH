// SPDX-License-Identifier: GPL-3.0-only
// Exercise the real firmware loop: unchanged pages must release the display bus.
import fs from 'node:fs';
import assert from 'node:assert/strict';
import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(process.argv[2] || 'build/controls/SMKSYNTH-1.0.3.wasm');
const seed=fs.readFileSync('web/simulator/factory-session.bin');
const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=(n,ms=100)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(150);};
const traffic=()=>[e.ex.sim_lcd_fills(),e.ex.sim_lcd_blits(),e.ex.sim_lcd_pixels()];
function idle(label){e.advance(2000);const a=traffic();e.advance(1000);const d=traffic().map((x,i)=>x-a[i]);assert.equal(d[0],0,label+' idle clear');assert(d[2]<=50000,label+' unnecessary idle traffic: '+d[2]);return d;}
const results=[];
for(const minutes of [0,3,10]){
 if(minutes)e.advance((minutes===3?3:7)*60000);
 b(12,900);const rows=[];
 for(const n of [0,0,0,15,15,12]){b(n);rows.push({button:n,page:e.ex.sim_page(),idle:idle('page '+e.ex.sim_page())});}
 // A page invalidation is consumed, but edits and notes still reach the UI and audio.
 b(12,900);const before=e.ex.sim_param(0,12);e.input({t:'enc',e:0,steps:before>64?-5:5});e.advance(150);assert.notEqual(e.ex.sim_param(0,12),before);
 e.input({t:'key',k:12,down:true});e.advance(100);assert(e.ex.sim_peak()>0);e.input({t:'key',k:12,down:false});e.advance(2000);
 assert.deepEqual(e.frame().writes,[0,0,0,0]);results.push({minutes,rows});
}
console.log(JSON.stringify({result:'PASS',idle_navigation:results,no_autosave:true,edits_and_notes:true}));
