// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';
import assert from 'node:assert/strict';
import {MidiInputController} from '../web/simulator/midi.js';
import {Engine} from '../web/simulator/engine.js';
const wasm=fs.readFileSync(new URL('../web/simulator/obervave-voice.wasm',import.meta.url));
const seed=fs.readFileSync(new URL('../web/simulator/factory-session.bin',import.meta.url));
function fixture(options={}){
 const sent=[],status=[],notes=[];let running=true,listening=true;
 const a={id:'a',name:'Keyboard A',state:'connected',async open(){},async close(){}};
 const b={id:'b',name:'Keyboard B',state:'connected',async open(){},async close(){}};
 const access={inputs:new Map([['a',a]])};let requests=0;
 const m=new MidiInputController({navigator:{async requestMIDIAccess(o){requests++;assert.deepEqual(o,{sysex:false});return access;}},secureContext:true,send:x=>sent.push(x),isRunning:()=>running,isListening:()=>listening,onStatus:(...x)=>status.push(x),onNote:(...x)=>notes.push(x),...options});
 return {m,a,b,access,sent,status,notes,get requests(){return requests;},run:x=>running=x,listen:x=>listening=x};
}
const f=fixture();await f.m.connect();assert.equal(f.m.input,f.a);assert.equal(f.requests,1);
f.a.onmidimessage({data:[0x92,127,47]});assert.deepEqual(f.sent.at(-1),{t:'midi',st:0x92,d1:127,d2:47});
f.a.onmidimessage({data:[0x92,127,0]});assert.deepEqual(f.sent.at(-1),{t:'midi',st:0x82,d1:127,d2:0});
f.m.message([0xb2,64,127]);assert.equal(f.sent.at(-1).d1,64);
f.m.message([0xe2,127,127]);assert.deepEqual(f.sent.at(-1),{t:'midi',st:0xe2,d1:127,d2:127});
f.m.message([0xb2,1,0]);assert.deepEqual(f.sent.at(-1),{t:'wheel',w:1,v:620});
f.m.message([0xb2,1,127]);assert.deepEqual(f.sent.at(-1),{t:'wheel',w:1,v:300});
let n=f.sent.length;for(const d of [[0xf8],[0xf0,1,0xf7],[0xc0,2],[0xb0,7,100],[0x90,60],[0x90,128,1]])f.m.message(d);assert.equal(f.sent.length,n);
f.run(false);f.m.message([0x90,60,100]);assert.equal(f.sent.length,n);f.run(true);
f.listen(false);f.m.message([0x90,60,100]);assert.equal(f.sent.length,n);
f.m.release();assert(f.sent.slice(n).some(x=>x.t==='midi'&&x.d1===64&&x.d2===0),'hidden tab must still release sustain');f.listen(true);
f.m.message([0x90,60,100]);f.m.message([0x91,60,100]);f.m.message([0x80,60,0]);assert.deepEqual(f.notes.at(-1),[60,true]);
f.access.inputs.set('b',f.b);f.access.onstatechange();assert.equal(f.m.input,f.a,'unrelated hotplug preserves held notes and selection');
await f.m.select('b');assert.equal(f.a.onmidimessage,null);assert.equal(f.m.input,f.b);assert.equal(f.m.notes.size,0);assert(f.sent.some(x=>x.t==='panic'));
f.b.state='disconnected';f.access.onstatechange();await Promise.resolve();assert.equal(f.m.input,null);assert.equal(f.m.selectedId,'');
await f.m.connect();assert.equal(f.requests,1,'reuse access instead of prompting again');assert.equal(f.m.input,f.a);
const unsupported=fixture({navigator:{}});await unsupported.m.connect();assert.equal(unsupported.status.at(-1)[0],'unsupported');
let denied=true;const retry=fixture({navigator:{async requestMIDIAccess(){if(denied)throw Object.assign(Error(),{name:'NotAllowedError'});return {inputs:new Map()};}}});await retry.m.connect();assert.equal(retry.status.at(-1)[0],'denied');denied=false;await retry.m.connect();assert.equal(retry.status.at(-1)[0],'empty');
// A superseded asynchronous port open must not steal a later selection.
const race=fixture();let finish;race.a.open=()=>new Promise(r=>finish=r);race.access.inputs.set('b',race.b);await race.m.connect();const pending=race.m.select('a');await Promise.resolve();await race.m.select('b');finish();await pending;assert.equal(race.m.input,race.b);assert.equal(race.a.onmidimessage,undefined);
// Actual frozen WASM: MIDI notes drive audio and retain the original note value in step entry.
const e=new Engine(wasm,seed.buffer.slice(seed.byteOffset,seed.byteOffset+seed.byteLength));e.advance(5000);
const live=fixture({send:x=>e.input(x)});await live.m.connect();
live.m.message([0x90,60,100]);e.advance(80);assert(e.ex.sim_gates_part(0,0)>0);assert(e.ex.sim_peak()>0);
live.m.message([0xb0,64,127]);live.m.message([0x80,60,0]);e.advance(80);
live.m.message([0xe0,127,127]);live.m.message([0xb0,1,127]);e.advance(80);assert(e.ex.sim_peak()>0);
live.m.release();e.advance(2500);assert.equal(e.ex.sim_melodic_busy(),0);
// Firmware SEQ/STEPS entry through the real MIDI queue, beyond the 37 virtual keys.
e.input({t:'btn',b:15,down:true});e.advance(80);e.input({t:'btn',b:15,down:false});e.advance(100);
// Navigate SEQ views until STEPS (view 1 in the firmware).
for(let i=0;i<8&&e.ex.sim_seq_view()!==1;i++){e.input({t:'btn',b:15,down:true});e.advance(80);e.input({t:'btn',b:15,down:false});e.advance(100);}
const m2=fixture({send:x=>e.input(x)});await m2.m.connect();m2.m.message([0x90,96,77]);e.advance(50);m2.m.message([0x80,96,0]);e.advance(150);
assert(Array.from({length:256},(_,n)=>e.ex.sim_step_note(0,n,0)).includes(96),'MIDI step entry keeps octave and full note range');
console.log('PASS: MIDI permissions/retry, selection/hotplug/races, channels/velocity/note-off, sustain, wheels, focus/panic, frozen WASM audio and full-range sequence entry');
