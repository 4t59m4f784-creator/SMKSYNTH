// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {webcrypto} from 'node:crypto';
import {PINS,validate,modeFor,readPending,PENDING} from '../experimental/pedal1/service.js';
if(!globalThis.crypto)globalThis.crypto=webcrypto;
for(const kind of ['release','update']){
 const pin=PINS[kind],raw=new Uint8Array(fs.readFileSync(new URL('../experimental/pedal1/'+pin.url,import.meta.url)));
 assert.equal((await validate(raw,kind)).product,pin.product);
 const bad=raw.slice();bad[100]^=1;await assert.rejects(()=>validate(bad,kind),/SHA/);
}
const id=text=>({model:text.split('_')[0],text});
assert.equal(modeFor('update',id('SMK-37 Pro_19618')),'install');
assert.equal(modeFor('update',id('SMK-37 Pro_19630')),'install');
assert.equal(modeFor('release',id('SMK-37 Pro_19630')),'install');
for(const source of ['SMK-37 Pro_016','SMK-37 Elite_19618','SMK-37 Pro_19628','FM-1_19618'])
 for(const kind of ['release','update'])assert.throws(()=>modeFor(kind,id(source)));
assert.throws(()=>modeFor('release',id('SMK-37 Pro_19618')));
assert.throws(()=>modeFor('update',id('ota-SMK-37 Pro_900')));
const pending={kind:'update',source:'SMK-37 Pro_19618',sha:PINS.update.sha};
assert.equal(modeFor('update',id('ota-SMK-37 Pro_900'),pending),'resume');
assert.equal(readPending({getItem:key=>key===PENDING?JSON.stringify(pending):null}).sha,PINS.update.sha);
console.log('PASS: PED1 and stable-return sizes/SHA/identity/CRCs, model/origin guards and matching interrupted-update resume; no MIDI hardware accessed.');
