// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(process.argv[2]),seed=fs.readFileSync('web/simulator/factory-session.bin');
const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const b=(n,ms=80)=>{e.input({t:'btn',b:n,down:true});e.advance(ms);e.input({t:'btn',b:n,down:false});e.advance(150);};const enc=(n,v)=>{e.input({t:'enc',e:n,steps:v});e.advance(150);};
const chars=' ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-',draft=()=>String.fromCharCode(...Array.from({length:10},(_,i)=>e.ex.sim_preset_name(i))),stored=n=>String.fromCharCode(...Array.from({length:10},(_,i)=>e.ex.sim_program_name(n,i)));
const shot=n=>{const fb=new Uint16Array(e.ex.memory.buffer,e.ex.sim_fb(),57600),out=Buffer.alloc(172800);for(let i=0;i<fb.length;i++){let v=fb[i];out[i*3]=(v>>11&31)*255/31;out[i*3+1]=(v>>5&63)*255/63;out[i*3+2]=(v&31)*255/31;}fs.writeFileSync('build/zm3/'+n+'.ppm',Buffer.concat([Buffer.from('P6\n240 240\n255\n'),out]));};
const type=name=>{for(let i=0;i<10;i++){enc(0,i-e.ex.sim_preset_cursor());let old=chars.indexOf(draft()[i]);enc(1,chars.indexOf(name[i])-(old<0?0:old));}assert.equal(draft(),name);};
for(let theme=0;theme<2;theme++){
 if(theme){for(let i=0;i<9&&e.ex.sim_page()!==31;i++)b(10);assert.equal(e.ex.sim_page(),31);enc(0,1);e.advance(2200);assert.equal(e.ex.sim_zoom_theme(),1);}
 b(12,800);const target=76+theme,t=e.ex.sim_track(),source=e.ex.sim_program(t),oldname=stored(source),write=e.ex.sim_writes(1);
 b(11,800);assert.equal(e.ex.sim_preset_save_state(),1);type(theme?'ZOOM BRASS':'WARM BRASS');const name=draft();enc(6,target-(e.ex.sim_preset_save_slot()-1));assert.equal(e.ex.sim_preset_save_slot(),target+1);assert.equal(draft(),name);assert.equal(e.ex.sim_program(t),source);assert.equal(stored(source),oldname);assert.equal(e.ex.sim_writes(1),write);shot('save-as-theme-'+theme);
 b(11);assert.equal(e.ex.sim_preset_save_state(),6);assert.equal(e.ex.sim_writes(1),write);shot('replace-theme-'+theme);enc(6,1);enc(1,1);assert.equal(e.ex.sim_preset_save_slot(),target+1);assert.equal(draft(),name);b(12);assert.equal(e.ex.sim_preset_save_state(),1);assert.equal(draft(),name);b(12);assert.equal(e.ex.sim_preset_save_state(),0);assert.equal(stored(source),oldname);assert.equal(e.ex.sim_writes(1),write);
 b(11,800);type(name);enc(6,target-(e.ex.sim_preset_save_slot()-1));b(11);assert.equal(e.ex.sim_preset_save_state(),6);b(11);assert.equal(e.ex.sim_preset_save_state(),2);shot('save-wait-theme-'+theme);e.advance(2500);assert.equal(e.ex.sim_preset_save_state(),3);shot('saved-theme-'+theme);assert.equal(e.ex.sim_program(t),target);assert.equal(stored(target),name);assert.equal(stored(source),oldname);e.advance(2400);
 const re=new Engine(wasm,e.store(true));re.advance(5000);assert.equal(String.fromCharCode(...Array.from({length:10},(_,i)=>re.ex.sim_program_name(target,i))),name);
 const prior=stored(target);b(11);assert.equal(e.ex.sim_preset_save_state(),2);assert.equal(e.ex.sim_preset_save_slot(),target+1);e.advance(5000);assert.equal(stored(target),prior);
}
console.log('PASS: real controls in both themes; ten-character typing, target/program visibility captures, draft survives K7/review/back, cancel writes nothing, distinct BT confirmation, source preserved, target name survives reboot, quick save keeps destination/name.');
