// SPDX-License-Identifier: GPL-3.0-only
export const SIZE=0x46000;
export function crc32(a){let c=0xffffffff;for(const v of a){c^=v;for(let i=0;i<8;i++)c=(c&1)?(c>>>1)^0xedb88320:c>>>1;}return (c^0xffffffff)>>>0;}
export function inventory(a){
 if(a.length!==SIZE||new TextDecoder().decode(a.slice(0,16))!=='SMK37DATASMK16P1')throw Error('Banco/layout não reconhecido.');
 const v=new DataView(a.buffer,a.byteOffset,a.byteLength),u=o=>v.getUint32(o,true),empty=(o,n)=>a.subarray(o,o+n).every(x=>x===255),records=[],warnings=[];
 const b64=b=>{let s='';for(const x of b)s+=String.fromCharCode(x);return btoa(s);};
 function sector(o,lengths){if(empty(o,4096))return null;const n=u(o+4);if(u(o)!==0x59474e44||!lengths.includes(n)||crc32(a.subarray(o+12,o+12+n))!==u(o+8))throw Error('Setor inválido: '+o.toString(16));return a.slice(o+12,o+12+n);}
 const global=sector(0x41000,[67]);if(global)records.push({type:'globals',schema:'set7',slot:0,data:b64(global)});
 for(let bank=0;bank<2;bank++){const p=sector(0x42000+bank*4096,[4056]);if(!p)continue;const dv=new DataView(p.buffer);if(dv.getUint32(0,true)!==0x50564f42)throw Error('Banco de presets inválido.');for(let j=0;j<50&&bank*50+j<99;j++){if(p[4+j]>1)throw Error('Ocupação de preset inválida.');if(p[4+j])records.push({type:'preset',schema:'patch80-kbo1-ove1',slot:bank*50+j,data:b64(p.slice(56+j*80,136+j*80))});}}
 for(let slot=0;slot<16;slot++){const o=4096+slot*16384;if(empty(o,16384))continue;const n=u(o+4),s=u(o+8);if(u(o)!==0x544f4c53||u(o+16)!==0x34514553||![13416,14448,14460,14476].includes(n)||s!==380||crc32(a.subarray(o+16,o+16+n+s))!==u(o+12))throw Error('Pattern '+(slot+1)+' incompleto ou corrompido.');records.push({type:'pattern',schema:'seq4-prefix-'+n,slot,data:b64(a.slice(o+16,o+16+n)),scene:b64(a.slice(o+16+n,o+16+n+s))});}
 const candidates=[];for(let bank=0;bank<2;bank++){try{const p=sector(0x44000+bank*4096,[700,708]);if(p){const d=new DataView(p.buffer);if(d.getUint32(0,true)!==(p.length===708?0x32564650:0x31564650)||(d.getUint32(8,true)&~255))throw Error('Favoritos inválidos.');candidates.push({p,g:d.getUint32(4,true)});}}catch(e){warnings.push(e.message);}}
 if(!candidates.length&&warnings.length)throw Error('Nenhuma cópia válida dos favoritos.');if(candidates.length){let c=candidates[0];for(const x of candidates)if((x.g-c.g|0)>0)c=x;records.push({type:'favorites',schema:c.p.length===708?'pfv2':'pfv1',slot:0,data:b64(c.p)});}
 return {records,warnings,presets:records.filter(x=>x.type==='preset').length,patterns:records.filter(x=>x.type==='pattern').length};
}
