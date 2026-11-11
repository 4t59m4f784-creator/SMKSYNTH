// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';import fs from 'node:fs';import os from 'node:os';import path from 'node:path';import {spawn} from 'node:child_process';import readline from 'node:readline';import {webcrypto} from 'node:crypto';if(!globalThis.crypto)Object.defineProperty(globalThis,'crypto',{value:webcrypto});
import {BankClient,digest,expand} from '../web/backup/client.js';import {parseFile} from '../web/backup/file.js';import {crc32,SIZE} from '../web/backup/bank.js';
const firmware=process.argv[3]||'19618',model=process.argv[4]||'SMK-37 Pro';
const root=new URL('../',import.meta.url),temp=fs.mkdtempSync(path.join(os.tmpdir(),'smk-b2-'));const backing=path.join(temp,'flash.bin');
function server(){const p=spawn(new URL('../build/backup_server',import.meta.url).pathname,[backing],{stdio:['pipe','pipe','inherit']}),lines=readline.createInterface({input:p.stdout}),q=[];let waiter;lines.on('line',s=>{q.push(Array.from(Buffer.from(s,'hex')));if(waiter){const w=waiter;waiter=null;w();}});let corrupt=false;return {client:new BankClient({send(a){assert.equal(a[0],240);assert.equal(a.at(-1),247);p.stdin.write(Buffer.from(a.slice(1,-1)).toString('hex')+'\n');return true;},async read(ms){if(!q.length){let tm;await Promise.race([new Promise(r=>waiter=r),new Promise(r=>tm=setTimeout(r,ms))]);clearTimeout(tm);waiter=null;}const a=q.shift();if(corrupt&&a){corrupt=false;a[a.length-3]^=1;}return a||null;}},firmware,model),corruptNext(){corrupt=true;},close(){return new Promise((resolve,reject)=>{p.on('exit',c=>c?reject(Error('server '+c)):resolve());p.stdin.end();});}};}
try{
 const target=new Uint8Array(fs.readFileSync(process.argv[2]||new URL('../build/b2-fixture-bank.bin',import.meta.url))),sha=await digest(target);
 const container={format:'smksynth-authoring-backup',version:1,layout:'SMK16P1',model,firmware,sha256:sha,bank:Buffer.from(target).toString('base64')},file={size:JSON.stringify(container).length,name:'B1.smksynth',text:async()=>JSON.stringify(container)};
 assert.deepEqual((await parseFile(file)).bank,target);container.sha256='0'.repeat(64);await assert.rejects(()=>parseFile(file),/SHA/);container.sha256=sha;
 let s=server();await s.client.check();const blank=await s.client.export();assert.equal(blank.length,SIZE);assert.equal(new TextDecoder().decode(blank.slice(0,16)),'SMK37DATASMK16P1');assert(s.client.stats.rxBytes<20000);const initialStats={...s.client.stats};
 s.corruptNext();await s.client.command(5);assert.equal(s.client.stats.retries,1);
 // Start and interrupt after staging a block. Restart actual firmware protocol state.
 const meta=new Uint8Array(36);meta.set(Buffer.from(sha,'hex'));new DataView(meta.buffer).setUint32(32,crc32(target),true);await s.client.command(8,0,36,meta);await s.client.command(9,0,512,target.subarray(0,512));await s.close();s=server();const pending=await s.client.state();assert(pending.pending&&pending.valid&&pending.sha===sha);
 const bad=meta.slice();bad[0]^=1;await assert.rejects(()=>s.client.command(8,0,36,bad),e=>e.code===8);
 await s.client.restore(target,sha);assert((await s.client.state()).complete);const actual=await s.client.readBank(()=>{},()=>false,true);assert.deepEqual(actual,target);const restoreStats={...s.client.stats};await s.close();s=server();assert(!(await s.client.state()).pending);assert.deepEqual(await s.client.export(),target);const exported={...s.client.stats};await s.close();
 assert.throws(()=>expand(Uint8Array.of(1,255,3),512));assert.throws(()=>expand(Uint8Array.of(1,0,3),1));assert.throws(()=>expand(Uint8Array.of(0,1),512));
 console.log('PASS: browser client <-> actual C firmware protocol, B1 file, SHA rejection, RLE export/CRC, corrupted reply retry, interrupted staging/restart, bound resume, full restore, flash SHA/readback and reboot state.');
 console.log(JSON.stringify({blankExport:initialStats,restoredBankExport:exported,restoreAndVerification:restoreStats}));
}finally{fs.rmSync(temp,{recursive:true,force:true});}

