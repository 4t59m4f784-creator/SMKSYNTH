// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';
const lab=new URL('../web/voice-lab/',import.meta.url);const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',lab)).toString('base64'));const wasm=fs.readFileSync(new URL('obervave-voice.wasm',lab));const e=new Engine(wasm);e.advance(5000);
const b=(n,ms=100)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(100);};
const pad=(p,ms=100)=>{e.input({t:'pad',p,v:100});e.advance(ms);e.input({t:'pad',p,v:0});e.advance(100);};
const traffic=()=>[e.ex.sim_lcd_fills(),e.ex.sim_lcd_blits(),e.ex.sim_lcd_pixels()];
b(11,900);assert.equal(e.frame().presetSave,1);let old=traffic();e.advance(2000);assert.deepEqual(traffic(),old);
const program=e.frame().programs[0];e.input({t:'enc',e:6,steps:98});e.advance(100);assert.equal(e.frame().presetSaveSlot,99);assert.equal(e.frame().programs[0],program);assert.equal(traffic()[0],old[0]);old=traffic();e.advance(800);assert.deepEqual(traffic(),old);b(12);
// Save slot16 and then ask overwrite. Dynamic updates are one narrow row, never a clear.
b(0);b(0);assert.equal(e.frame().bank,2);pad(15,850);e.advance(7000);assert(e.frame().patternsUsed&32768);pad(15,850);assert.equal(e.frame().patternConfirm,16);old=traffic();e.advance(1000);const now=traffic();assert.equal(now[0],old[0]);assert.equal(now[1]-old[1],1);assert.equal(now[2]-old[2],240*20);e.advance(3100);assert.equal(e.frame().patternConfirm,0);
console.log('PASS: real firmware loop preset dialog zero LCD traffic while idle, destination99 without program load, no clear on selection, pad16 timeout count only4800 pixels/second, confirmation canceled at4s. Physical TFT validation pending.');
