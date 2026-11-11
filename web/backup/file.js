// SPDX-License-Identifier: GPL-3.0-only
import {supportedFirmware} from './compatibility.js?v=backup-elite2';
import {inventory} from './bank.js';
import {digest} from './client.js?v=backup-elite2';
export async function parseFile(f){
 if(f.size>2*1024*1024)throw Error('Arquivo grande demais para este formato.');
 const j=JSON.parse(await f.text());
 if(j.format!=='smksynth-authoring-backup'||j.version!==1||j.layout!=='SMK16P1'||!['SMK-37 Pro','SMK-37 Elite'].includes(j.model)||!supportedFirmware(j.firmware,j.model)||!/^[a-f0-9]{64}$/.test(j.sha256))throw Error('Formato/versão não aceito.');
 if(typeof j.bank!=='string'||!/^[A-Za-z0-9+/]*={0,2}$/.test(j.bank))throw Error('Base64 inválido.');
 const s=atob(j.bank),bank=Uint8Array.from(s,x=>x.charCodeAt(0)),index=inventory(bank);
 if(await digest(bank)!==j.sha256)throw Error('SHA-256 não confere.');
 return {bank,sha:j.sha256,index,name:f.name};
}

