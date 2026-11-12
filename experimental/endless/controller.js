// SPDX-License-Identifier: GPL-3.0-only
import {InstallService,PINS,validate,modeFor,readPending} from './service.js?v=sp2-m1';
const $=id=>document.getElementById(id);
const buffers={};
let service=null,device=null,busy=false,choice=null;
const lines=['SMKSYNTH installer · nenhum dispositivo acessado ao abrir a página.'];
function log(s){lines.push(new Date().toISOString()+' '+s);$('log').textContent=lines.join('\n');$('log').scrollTop=$('log').scrollHeight}
function status(id,s){$(id).textContent=s}
function mode(kind){try{return modeFor(kind,device,readPending(localStorage))}catch{return null}}
function refresh(){
 for(const id of Object.keys(PINS))$(id).disabled=busy||Object.keys(buffers).length!==Object.keys(PINS).length||!service||!mode(id);
 $('connect').disabled=busy;
}
async function fetchPackages(){try{
 for(const kind of Object.keys(PINS)){
  const r=await fetch(PINS[kind].url,{cache:'no-store'});if(!r.ok)throw Error('Download indisponível: '+kind);
  const raw=new Uint8Array(await r.arrayBuffer());await validate(raw,kind);buffers[kind]=raw;
 }
 status('packageStatus','SUPER SP2 e retorno oficial verificados: SHA-256, modelo e CRCs aprovados.');
}catch(e){for(const kind of Object.keys(buffers))delete buffers[kind];status('packageStatus','Pacotes bloqueados: '+e.message);log(e.message)}refresh()}
$('connect').addEventListener('click',async()=>{
 if(busy)return;device=null;service=null;refresh();$('connect').disabled=true;status('deviceStatus','Solicitando acesso USB-MIDI e consultando identidade…');
 try{
  if(!navigator.requestMIDIAccess)throw Error('Use Chrome ou Edge no computador, em HTTPS.');
  const access=await navigator.requestMIDIAccess({sysex:true});
  service=new InstallService(access,localStorage,log);device=await service.identify();
  status('deviceStatus','Identidade informada: '+device.text);log('Consulta: '+device.text);
  const allowed=Object.keys(PINS).some(k=>mode(k));status('operationStatus',allowed?'Teclado identificado. Escolha a operação e confira a confirmação.':'Identidade não habilitada para esta página; nenhuma atualização foi iniciada.');
  access.onstatechange=()=>{if(!busy){device=null;status('deviceStatus','A conexão MIDI mudou. Consulte o teclado novamente.');refresh()}};
 }catch(e){device=null;service=null;status('deviceStatus',e.message);log('Consulta falhou: '+e.message)}refresh();
});
function prepare(kind){
 if(busy||Object.keys(buffers).length!==Object.keys(PINS).length||!service||!mode(kind))return;choice=kind;
 $('confirmTitle').textContent=(mode(kind)==='resume'?'Retomar: ':'')+PINS[kind].name;
 $('confirmDetails').textContent='Origem: '+device.text+'. Destino: '+PINS[kind].product+'. '+(kind==='update'?'Instala o protótipo SUPER SP2 mantendo o banco atual. Exporte um backup antes do teste.':'Retorna à versão oficial 1.1.0 preservando o banco e os ajustes SUPER.')+' Se houver restauração pendente, conclua-a com o mesmo arquivo antes de atualizar.';
 $('confirmWrite').textContent='Confirmar firmware';$('confirmation').showModal();
}
for(const kind of Object.keys(PINS))$(kind).addEventListener('click',()=>prepare(kind));
$('confirmation').addEventListener('close',async()=>{
 const kind=choice;choice=null;if($('confirmation').returnValue!=='confirm'||!kind||busy)return;
 busy=true;refresh();$('progress').removeAttribute('value');status('operationStatus','Conferindo novamente e iniciando a operação confirmada…');
 const run=async()=>{
  const labels={start:'Atualização confirmada',verify:'Teclado conferindo o pacote',loader:'Aguardando modo de atualização',write:'Gravando',reboot:'Aguardando reinicialização',done:'Identidade de destino confirmada'};
  const result=await service.execute(kind,buffers,(step,n)=>{const s=(labels[step]||step)+(n!==undefined?' · '+n:'');status('operationStatus',s);if(step!=='write'&&step!=='verify'||Number(n)%100===0)log(s)});
  device={text:result,model:'SMK-37 Pro',version:Number(PINS[kind].product.split('_')[1])};
  $('progress').value=1;status('deviceStatus','Identidade após reiniciar: '+result);
  status('operationStatus','Concluído: o teclado informa '+result+'. Confira a versão na inicialização e teste o instrumento.');log('Concluído: '+result);
 };
 try{
  if(!navigator.locks?.request)throw Error('Este navegador não oferece o bloqueio necessário entre abas. Use Chrome ou Edge atualizado.');
  await navigator.locks.request('obervave-smk37-firmware-update',{ifAvailable:true},async lock=>{if(!lock)throw Error('Outra aba deste site está usando o atualizador.');await run()});
 }catch(e){device=null;$('progress').value=0;status('operationStatus','Operação não concluída: '+e.message+' Baixe o log antes de tentar outro procedimento.');log('ERRO: '+(e.stack||e.message))}
 finally{busy=false;refresh()}
});
window.addEventListener('beforeunload',e=>{if(busy){e.preventDefault();e.returnValue=''}});
$('downloadLog').addEventListener('click',()=>{const url=URL.createObjectURL(new Blob([lines.join('\n')+'\n'],{type:'text/plain'}));const a=document.createElement('a');a.href=url;a.download='smksynth-super-sp2-installer-log.txt';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000)});
refresh();fetchPackages();

