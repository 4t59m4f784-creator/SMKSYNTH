// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {webcrypto} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import vm from 'node:vm';
import {B2_FIRMWARES,canRestore,supportedFirmware,supportedIdentity} from '../web/backup/compatibility.js';
import {BankClient,digest} from '../web/backup/client.js';
import {parseFile} from '../web/backup/file.js';
if(!globalThis.crypto)Object.defineProperty(globalThis,'crypto',{value:webcrypto});
const bank=new Uint8Array(fs.readFileSync(new URL('../factory/bank-SMK16P1.bin',import.meta.url)));
const sha256=await digest(bank);
const file=(firmware,extra={})=>{const data={format:'smksynth-authoring-backup',version:1,layout:'SMK16P1',model:'SMK-37 Pro',firmware,sha256,bank:Buffer.from(bank).toString('base64'),...extra};const text=JSON.stringify(data);return {name:'test.smksynth',size:text.length,text:async()=>text};};
for(const firmware of ['19614',...B2_FIRMWARES]){
 assert(supportedFirmware(firmware));
 assert(supportedIdentity({model:'SMK-37 Pro',text:'SMK-37 Pro_'+firmware}));
 assert.deepEqual((await parseFile(file(firmware))).bank,bank);
 assert.equal(canRestore(firmware),firmware!=='19614');
 const client=new BankClient({},firmware),commands=[];
 client.command=async cmd=>{commands.push(cmd);if(cmd===1){const info=new Uint8Array(11);info.set(new TextEncoder().encode('SMK16P1'));new DataView(info.buffer).setUint32(7,bank.length,true);return info;}if(cmd===5)return new TextEncoder().encode('B2R1');throw Error('Unexpected command');};
 await client.check();assert.deepEqual(commands,firmware==='19614'?[1]:[1,5]);
 if(firmware!=='19614'){
  client.command=async cmd=>cmd===1?Uint8Array.of(...new TextEncoder().encode('SMK16P1'),0,96,4,0):new TextEncoder().encode('BAD!');
  await assert.rejects(()=>client.check(),/B2/);
  await assert.rejects(()=>client.restore(bank,sha256),/B2/);
 }
}
for(const firmware of ['19613','19629','99999','19628_extra']){
 assert(!supportedFirmware(firmware));
 assert(!supportedIdentity({model:'SMK-37 Pro',text:'SMK-37 Pro_'+firmware}));
 await assert.rejects(()=>parseFile(file(firmware)),/Formato/);
 const client=new BankClient({},firmware);client.command=()=>assert.fail('Unknown firmware sent a command');
 await assert.rejects(()=>client.check(),/compatível/);
 await assert.rejects(()=>client.restore(bank,sha256),/B2/);
}
for(const model of ['SMK-37 Elite','FM-1','ota-SMK-37 Pro'])assert(!supportedIdentity({model,text:model+'_19628'}));
await assert.rejects(()=>parseFile(file('19628',{sha256:'0'.repeat(64)})),/SHA/);
await assert.rejects(()=>parseFile(file('19628',{layout:'other'})),/Formato/);
await assert.rejects(()=>parseFile(file('19628',{model:'SMK-37 Elite'})),/Formato/);
// Exercise the real page's Connect handler as well as its shared compatibility rules.
const app=fs.readFileSync(new URL('../web/backup/app.js',import.meta.url),'utf8').replace(/^import .*$/gm,'');
for(const firmware of ['19614',...B2_FIRMWARES,'19629']){
 const elements=new Map();const get=id=>{if(!elements.has(id))elements.set(id,{disabled:false,textContent:''});return elements.get(id);};
 let checks=0;
 vm.runInNewContext(app,{canRestore,supportedIdentity,document:{getElementById:get},window:{addEventListener(){}},navigator:{requestMIDIAccess:async()=>({})},
  Updater:class{async find(filter){const id={model:'SMK-37 Pro',text:'SMK-37 Pro_'+firmware};return filter(id)?{id,link:{}}:null;}},
  BankClient:class{async check(){checks++;}async state(){return null;}}
 });
 await get('connect').onclick();
 assert.equal(checks,supportedFirmware(firmware)?1:0);
 assert.equal(!!get('export').disabled,!supportedFirmware(firmware));
 assert(get('restore').disabled,'Restore must require a file and a fresh backup');
 if(supportedFirmware(firmware))assert(get('device').textContent.includes(firmware));
 else assert(get('status').textContent.includes('SMK-37 Pro_'+firmware),'Show the rejected identity');
}
console.log('PASS: stable/SS/UI/ENDLESS/SP1 identities and files; B2 signature, unknown identity, model, layout and SHA guards before restore writes');
if(process.argv.includes('--protocol'))for(const firmware of B2_FIRMWARES){
 execFileSync(process.execPath,[new URL('./release_backup_client_test.mjs',import.meta.url).pathname,new URL('../factory/bank-SMK16P1.bin',import.meta.url).pathname,firmware],{stdio:'pipe'});
 console.log(`PASS: ${firmware} full export, CRC retry, interrupted restore/resume and exact bank readback`);
}
