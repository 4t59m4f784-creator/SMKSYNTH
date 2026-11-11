// SPDX-License-Identifier: GPL-3.0-only
import {InstallService,PINS,validate,modeFor,readPending} from './service.js?v=elite2';
import {applyLanguage,lang,t} from './language.js?v=elite2';
applyLanguage();
const $=id=>document.getElementById(id),buffers={},lines=[];
let service,device,busy=false,choice;
$('backupLink').href='../backup/?lang='+lang;
if(lang==='en')document.querySelector('a[href="README.md"]').href='README.en.md';
function log(s){lines.push(new Date().toISOString()+' '+s);$('log').textContent=lines.join('\n');}
function mode(kind){try{return modeFor(kind,device,readPending(localStorage))}catch{return null}}
function refresh(){for(const kind of Object.keys(PINS))$(kind).disabled=busy||!buffers[kind]||!service||!mode(kind);$('connect').disabled=busy;$('stockFile').disabled=busy;}
async function fetchPackages(){
 try{for(const kind of ['release','update']){const response=await fetch(PINS[kind].url,{cache:'no-store'});if(!response.ok)throw Error(t('Download indisponível.','Download unavailable.'));const raw=new Uint8Array(await response.arrayBuffer());await validate(raw,kind);buffers[kind]=raw;}
 $('packageStatus').textContent=t('Pacotes Elite verificados: SHA-256, modelo e CRCs aprovados.','Elite packages verified: SHA-256, model and CRC checks passed.');}
 catch(e){delete buffers.release;delete buffers.update;$('packageStatus').textContent=e.message;log(e.message);}refresh();
}
$('connect').onclick=async()=>{
 if(busy)return;device=service=null;busy=true;refresh();
 try{if(!navigator.requestMIDIAccess)throw Error(t('Use Chrome ou Edge no computador, em HTTPS.','Use desktop Chrome or Edge over HTTPS.'));
 const access=await navigator.requestMIDIAccess({sysex:true});service=new InstallService(access,localStorage,log);device=await service.identify();$('deviceStatus').textContent=device.text;log('Identity: '+device.text);
 $('operationStatus').textContent=Object.keys(PINS).some(k=>mode(k))?t('Confira a operação e confirme antes de gravar.','Check the operation and confirm before writing.'):t('Identidade não habilitada. Nenhuma gravação iniciada.','Identity not enabled. No write started.');
 access.onstatechange=()=>{if(!busy){device=null;refresh();$('deviceStatus').textContent=t('USB mudou. Consulte novamente.','USB changed. Identify again.');}};
 }catch(e){device=service=null;$('deviceStatus').textContent=e.message;log(e.message);}finally{busy=false;refresh();}
};
$('stockFile').onchange=async()=>{delete buffers.stock;refresh();try{const file=$('stockFile').files[0];if(!file)return;if(file.size!==PINS.stock.size)throw Error(t('Arquivo oficial com tamanho incorreto.','Incorrect stock file size.'));const raw=new Uint8Array(await file.arrayBuffer());await validate(raw,'stock');buffers.stock=raw;$('stockStatus').textContent=t('Elite V016 oficial verificado.','Elite stock V016 verified.');}catch(e){$('stockStatus').textContent=e.message;}refresh();};
function prepare(kind){
 if($(kind).disabled)return;choice=kind;$('confirmTitle').textContent=$(kind).textContent;
 const details=kind==='release'?t('Substitui o banco e instala cinco patterns e oito favoritos.','Replaces the bank and installs five patterns and eight favourites.'):kind==='update'?t('Preserva o banco salvo. Exporte um backup antes.','Preserves the saved bank. Export a backup first.'):t('Retorna ao firmware oficial. O banco SMKSYNTH pode ser substituído. Exporte um backup antes.','Returns to stock firmware. The SMKSYNTH bank may be replaced. Export a backup first.');
 $('confirmDetails').textContent=t('Origem: ','Source: ')+device.text+t('. Destino: ','. Target: ')+PINS[kind].product+'. '+details;
 $('phrase').value='';$('confirmWrite').disabled=true;$('confirmation').showModal();
}
for(const kind of Object.keys(PINS))$(kind).onclick=()=>prepare(kind);
$('phrase').oninput=()=>$('confirmWrite').disabled=$('phrase').value!=='ELITE';
$('confirmation').onclose=async()=>{
 const kind=choice;choice=null;if($('confirmation').returnValue!=='confirm'||$('phrase').value!=='ELITE'||!kind||busy)return;
 busy=true;refresh();$('progress').removeAttribute('value');
 try{if(!navigator.locks?.request)throw Error(t('Use Chrome/Edge com suporte a bloqueio entre abas.','Use Chrome/Edge with cross-tab locking support.'));
 await navigator.locks.request('obervave-smk37-firmware-update',{ifAvailable:true},async lock=>{if(!lock)throw Error(t('Outra aba está usando o teclado.','Another tab is using the keyboard.'));
 const labels={start:t('Operação confirmada','Operation confirmed'),verify:t('Conferindo pacote','Verifying package'),loader:t('Aguardando loader','Waiting for loader'),write:t('Gravando','Writing'),reboot:t('Aguardando reinicialização','Waiting for reboot'),done:t('Identidade confirmada','Identity confirmed')};
 const result=await service.execute(kind,buffers,(step,n)=>{const message=(labels[step]||step)+(n===undefined?'':' · '+n);$('operationStatus').textContent=message;if(step!=='write'&&step!=='verify'||Number(n)%100===0)log(message);});
 device={text:result,model:'SMK-37 Elite'};$('deviceStatus').textContent=result;$('progress').value=1;$('operationStatus').textContent=t('Concluído. Confira a versão e teste o instrumento.','Complete. Check the version and test the instrument.');log('Complete: '+result);});
 }catch(e){device=null;$('progress').value=0;$('operationStatus').textContent=t('Não concluído. Guarde o log e retome a mesma operação. Detalhe: ','Incomplete. Keep the log and resume the same operation. Detail: ')+e.message;log(e.stack||e.message);}finally{busy=false;refresh();}
};
$('downloadLog').onclick=()=>{const url=URL.createObjectURL(new Blob([lines.join('\n')],{type:'text/plain'})),a=document.createElement('a');a.href=url;a.download='smksynth-elite2-log.txt';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);};
window.addEventListener('beforeunload',e=>{if(busy){e.preventDefault();e.returnValue='';}});
refresh();fetchPackages();
