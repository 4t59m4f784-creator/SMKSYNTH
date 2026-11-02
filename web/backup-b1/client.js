// SPDX-License-Identifier: GPL-3.0-only
import {crc32,SIZE} from './bank.js';
const prefix=[125,83,77,75,66,1];
export function unpack(a){let acc=0,bits=0,out=[];for(const v of a){acc|=v<<bits;bits+=7;if(bits>=8){out.push(acc&255);acc>>>=8;bits-=8;}}return Uint8Array.from(out);}
export class BackupClient{
 constructor(link){this.link=link;this.seq=0;}
 async command(cmd,off=0,n=0){const seq=++this.seq&16383,r=[...prefix,cmd,seq&127,seq>>7,off&127,off>>7&127,off>>14&127,n&127,n>>7];r.push(r.reduce((a,b)=>a^b,0));
 for(let retry=0;retry<3;retry++){if(!this.link.send([240,...r,247]))throw Error('USB desconectado.');const deadline=Date.now()+1500;while(Date.now()<deadline){const msg=await this.link.read(Math.max(1,deadline-Date.now()));if(!msg)break;const a=msg.slice(1,-1);if(msg[0]!==240||msg.at(-1)!==247||!prefix.every((v,i)=>a[i]===v)||a[6]!==(cmd|64)||a[7]!==r[7]||a[8]!==r[8])continue;if(a[9])throw Error(['','Comando inválido.','Pare PLAY/REC, solte teclas e sustain e aguarde os saves terminarem.','Sessão expirou. Exporte novamente.','Faixa recusada.','Erro de leitura.'][a[9]]||'Falha de transferência.');const len=a[13]|a[14]<<7,encoded=Math.ceil(len*8/7);if(a.length!==15+encoded+5)continue;const data=unpack(a.slice(15,15+encoded)),c=unpack(a.slice(15+encoded)),want=new DataView(c.buffer).getUint32(0,true);if(data.length!==len||crc32(data)!==want||(a[10]|a[11]<<7|a[12]<<14)!==off)continue;return data;}}
 throw Error('Sem resposta válida; nenhuma gravação foi solicitada.');}
 async export(progress=()=>{},cancel=()=>false){const info=await this.command(1);if(new TextDecoder().decode(info.slice(0,7))!=='SMK16P1'||new DataView(info.buffer).getUint32(7,true)!==SIZE)throw Error('Layout incompatível.');await this.command(2);let first;
 try{for(let pass=0;pass<2;pass++){const data=new Uint8Array(SIZE);for(let off=0;off<SIZE;off+=128){if(cancel())throw Error('Exportação cancelada.');const b=await this.command(3,off,128);if(b.length!==128)throw Error('Bloco incompleto.');data.set(b,off);progress((pass*SIZE+off+128)/(2*SIZE));}if(!pass)first=data;else if(!data.every((v,i)=>v===first[i]))throw Error('As duas leituras divergem; arquivo não criado.');}return first;}finally{await this.command(4).catch(()=>{});}}
}
