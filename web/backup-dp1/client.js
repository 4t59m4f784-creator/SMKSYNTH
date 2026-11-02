// SPDX-License-Identifier: GPL-3.0-only
import {BackupClient,unpack} from '../backup-b1/client.js';
import {crc32,SIZE} from './bank.js';
const prefix=[125,83,77,75,66,1];
export const hex=a=>Array.from(a,x=>x.toString(16).padStart(2,'0')).join('');
export const digest=async a=>hex(new Uint8Array(await crypto.subtle.digest('SHA-256',a)));
function pack(a){let v=0,bits=0,out=[];for(const b of a){v|=b<<bits;bits+=8;while(bits>=7){out.push(v&127);v>>>=7;bits-=7;}}if(bits)out.push(v);return out;}
const le=n=>Uint8Array.of(n&255,n>>>8&255,n>>>16&255,n>>>24);
export function expand(a,n){if(!a.length)throw Error('Bloco vazio.');if(a[0]===0){if(a.length!==n+1)throw Error('Bloco incompleto.');return a.slice(1);}if(a[0]!==1||(a.length-1)%2)throw Error('Compressão inválida.');const out=new Uint8Array(n);let k=0;for(let i=1;i<a.length;i+=2){if(!a[i]||k+a[i]>n)throw Error('Run inválido.');out.fill(a[i+1],k,k+a[i]);k+=a[i];}if(k!==n)throw Error('Run incompleto.');return out;}
export class BankClient extends BackupClient{
 constructor(link,firmware='19615'){super(link);this.firmware=firmware;this.stats={commands:0,retries:0,rxBytes:0,txBytes:0};}
 async command(cmd,off=0,n=0,payload=null){
  const seq=++this.seq&16383,r=[...prefix,cmd,seq&127,seq>>7,off&127,off>>7&127,off>>14&127,n&127,n>>7];r.push(r.reduce((a,b)=>a^b,0));
  const extra=payload?[...pack(payload),...pack(le(crc32(payload)))]:[],wire=[240,...r,...extra,247];
  this.stats.commands++;
  for(let retry=0;retry<3;retry++){
   if(retry)this.stats.retries++;this.stats.txBytes+=wire.length;
   if(!this.link.send(wire))throw Error('USB desconectado.');const deadline=Date.now()+2500;
   while(Date.now()<deadline){const msg=await this.link.read(Math.max(1,deadline-Date.now()));if(!msg)break;this.stats.rxBytes+=msg.length;const a=msg.slice(1,-1);
    if(msg[0]!==240||msg.at(-1)!==247||!prefix.every((v,i)=>a[i]===v)||a[6]!==(cmd|64)||a[7]!==r[7]||a[8]!==r[8])continue;
    const len=a[13]|a[14]<<7,enc=Math.ceil(len*8/7);if(len>513||a.length!==15+enc+5||(a[10]|a[11]<<7|a[12]<<14)!==off)continue;
    const data=unpack(a.slice(15,15+enc)),c=unpack(a.slice(15+enc));if(data.length!==len||c.length!==4||crc32(data)!==new DataView(c.buffer).getUint32(0,true))continue;
    if(a[9]){const e=Error(({1:'Comando ou checksum inválido.',2:'Pare PLAY/REC, solte teclas e sustain e aguarde os saves.',3:'Sessão indisponível ou expirada.',4:'Faixa/tamanho recusado.',5:'Falha de flash. Retome com o mesmo arquivo.',6:'Verificação da flash falhou.',8:'Restauração pendente de outro arquivo. Selecione o arquivo original.',9:'Setor ainda precisa ser enviado.'})[a[9]]||'Falha de transferência.');e.code=a[9];throw e;}
    return data;
   }
  }throw Error('Sem resposta válida. Se a restauração já começou, retome com o mesmo arquivo.');
 }
 async check(){const info=await this.command(1);if(new TextDecoder().decode(info.slice(0,7))!=='SMK16P1'||info.length!==11||new DataView(info.buffer,info.byteOffset).getUint32(7,true)!==SIZE)throw Error('Layout incompatível.');if(['19615','19616'].includes(this.firmware)&&new TextDecoder().decode(await this.command(5))!=='B2R1')throw Error('B2 incompatível.');}
 async readBank(progress=()=>{},cancel=()=>false,verify=true){const bank=new Uint8Array(SIZE);for(let off=0;off<SIZE;off+=512){if(cancel())throw Error('Exportação cancelada.');bank.set(expand(await this.command(6,off,512),512),off);progress((off+512)/SIZE*(verify?0.85:1));}if(verify)for(let off=0;off<SIZE;off+=4096){if(cancel())throw Error('Exportação cancelada.');const c=await this.command(7,off,4096);if(c.length!==4||new DataView(c.buffer,c.byteOffset).getUint32(0,true)!==crc32(bank.subarray(off,off+4096)))throw Error('Leitura e conferência divergem; arquivo não criado.');progress(.85+.15*(off+4096)/SIZE);}return bank;}
 async export(progress=()=>{},cancel=()=>false){if(this.firmware==='19614')return super.export(progress,cancel);await this.check();await this.command(2);try{return await this.readBank(progress,cancel);}finally{await this.command(4).catch(()=>{});}}
 async state(){if(!['19615','19616'].includes(this.firmware))return null;const a=await this.command(12);if(a.length!==105)throw Error('Estado incompleto.');return {pending:!!a[0],valid:!!a[1],complete:!!a[2],sha:hex(a.slice(3,35)),marks:a.slice(35)};}
 async restore(bank,sha,progress=()=>{}){
  if(!['19615','19616'].includes(this.firmware))throw Error('Restauração exige B2.');await this.check();
  if(bank.length!==SIZE||await digest(bank)!==sha)throw Error('Banco não corresponde ao hash.');
  const meta=new Uint8Array(36);meta.set(Uint8Array.from(sha.match(/../g),x=>parseInt(x,16)));meta.set(le(crc32(bank)),32);
  await this.command(8,0,36,meta);const state=await this.state();if(!state.pending||!state.valid||state.sha!==sha)throw Error('Journal não corresponde ao arquivo.');
  for(let sector=0;sector<70;sector++){
   const off=sector*4096,part=bank.subarray(off,off+4096),c=le(crc32(part));
   // Even marked sectors are checked by the device; skip only exact matches.
   try{await this.command(10,off,4,c);}catch(e){if(e.code!==9)throw e;for(let n=0;n<4096;n+=512)await this.command(9,off+n,512,part.subarray(n,n+512));await this.command(10,off,4,c);}
   progress((sector+1)/70*.7);
  }
  const actual=await this.readBank(p=>progress(.7+p*.3),()=>false,false);
  if(await digest(actual)!==sha)throw Error('SHA-256 da flash difere do arquivo. Não reinicie para tocar; retome a restauração.');
  await this.command(11);return {sha,stats:this.stats};
 }
}
