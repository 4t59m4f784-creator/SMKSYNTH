// SPDX-License-Identifier: GPL-3.0-only
import {InstallService,PINS,validate,chooseOperation,readPending} from './service.js?v=120';
import {applyLanguage,t} from './language.js';
applyLanguage();
const $=id=>document.getElementById(id),buffers={},lines=[];
let service,device,selection,busy=false,ready=false;
function log(s){lines.push(new Date().toISOString()+' '+s);$('log').textContent=lines.join('\n');}
function refresh(){$('connect').disabled=busy;$('install').disabled=busy||!ready||!selection||!service;}
async function packages(){try{for(const model of Object.keys(PINS)){buffers[model]={};for(const kind of ['release','update']){const r=await fetch(PINS[model][kind].url,{cache:'no-store'});if(!r.ok)throw Error(t('Pacote indisponível.','Package unavailable.'));const raw=new Uint8Array(await r.arrayBuffer());await validate(raw,model,kind);buffers[model][kind]=raw;}}ready=true;$('packageStatus').textContent=t('Pacotes Pro e Elite verificados. O modelo será identificado automaticamente.','Pro and Elite packages verified. Your model will be identified automatically.');}catch(e){ready=false;$('packageStatus').textContent=e.message;log(e.message);}refresh();}
$('connect').onclick=async()=>{
 if(busy)return;device=selection=service=null;busy=true;refresh();
 try{if(!navigator.requestMIDIAccess)throw Error(t('Use Chrome ou Edge no computador.','Use desktop Chrome or Edge.'));const access=await navigator.requestMIDIAccess({sysex:true});service=new InstallService(access,localStorage,log);device=await service.identify();$('deviceStatus').textContent=device.text;selection=chooseOperation(device,readPending(localStorage));const name=PINS[selection.model].model;
 $('install').textContent=(selection.mode==='resume'?t('Retomar','Resume'):selection.kind==='release'?t('Instalar','Install'):t('Atualizar','Update'))+' SMKSYNTH 1.2.0 · '+name;
 $('operationStatus').textContent=selection.kind==='release'?t('Primeira instalação: prepara cinco patterns e oito favoritos e substitui o banco anterior.','First installation: prepares five patterns and eight favourites and replaces the previous bank.'):t('Atualização: preserva seus programas, favoritos, patterns e configurações salvas. Exporte um backup antes.','Update: preserves your saved programs, favourites, patterns and settings. Export a backup first.');
 access.onstatechange=()=>{if(!busy){device=selection=null;$('deviceStatus').textContent=t('USB mudou. Conecte e consulte novamente.','USB changed. Connect and identify again.');refresh();}};
 }catch(e){selection=null;$('deviceStatus').textContent=e.message;log(e.message);}finally{busy=false;refresh();}
};
$('install').onclick=()=>{if($('install').disabled)return;$('confirmTitle').textContent=$('install').textContent;$('confirmDetails').textContent=t('Origem: ','Source: ')+selection.source+t('. Destino: ','. Target: ')+PINS[selection.model][selection.kind].product+'. '+$('operationStatus').textContent;$('confirmation').showModal();};
$('confirmation').onclose=async()=>{
 if($('confirmation').returnValue!=='confirm'||!selection||busy)return;busy=true;refresh();$('progress').removeAttribute('value');
 try{if(!navigator.locks?.request)throw Error(t('Use Chrome ou Edge atualizado.','Use an up-to-date Chrome or Edge.'));await navigator.locks.request('obervave-smk37-firmware-update',{ifAvailable:true},async lock=>{if(!lock)throw Error(t('Outra aba está usando o teclado.','Another tab is using the keyboard.'));const result=await service.execute(selection,buffers,(phase,n)=>{$('operationStatus').textContent=t('Atualizando: ','Updating: ')+phase+(n===undefined?'':' · '+n);if(!['write','verify'].includes(phase)||Number(n)%100===0)log(phase+' '+(n??''));});$('progress').value=1;$('deviceStatus').textContent=result;$('operationStatus').textContent=t('Concluído. Reinicie com o pedal conectado e solto e confira os novos recursos.','Complete. Restart with your pedal connected and released, then try the new features.');selection=null;});}
 catch(e){selection=null;$('operationStatus').textContent=e.message;log(e.message);}finally{busy=false;refresh();}
};
$('downloadLog').onclick=()=>{const u=URL.createObjectURL(new Blob([lines.join('\n')],{type:'text/plain'})),a=document.createElement('a');a.href=u;a.download='smksynth-1.2.0-install-log.txt';a.click();setTimeout(()=>URL.revokeObjectURL(u),1000);};
window.addEventListener('beforeunload',e=>{if(busy){e.preventDefault();e.returnValue='';}});refresh();packages();
