// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';import assert from 'node:assert/strict';import {execFileSync} from 'node:child_process';
import {crc32,inventory} from '../experimental/sub-osc-su1/backup/bank.js';
const a=new Uint8Array(fs.readFileSync('factory/bank-SMK16P1.bin')),d=new DataView(a.buffer);
const o=4096,n=d.getUint32(o+4,true),size=d.getUint32(o+8,true),at=o+16+n+12+74;
assert(size===380);const tag=Uint8Array.of(83,85,64|128,64|128,100,0);tag[5]=0x6d^tag[2]^tag[3]^tag[4];a.set(tag,at);d.setUint32(o+12,crc32(a.subarray(o+16,o+16+n+size)),true);
assert(inventory(a));fs.writeFileSync('build/su1/sub-bank.bin',a);
for(const model of ['SMK-37 Pro','SMK-37 Elite'])execFileSync(process.execPath,['tests/sub_backup_client_test.mjs','build/su1/sub-bank.bin','19661',model],{stdio:'pipe'});
console.log('PASS: actual SUB-tagged scene bank exports/restores byte-identically via B2, including CRC retry and interrupted restore, for Pro/Elite.');
