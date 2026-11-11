// SPDX-License-Identifier: GPL-3.0-only
let saved;try{saved=globalThis.localStorage?.getItem('smksynth-language')}catch{}
export const lang=(new URLSearchParams(globalThis.location?.search||'').get('lang')||saved)==='en'?'en':'pt';
export const t=(pt,en)=>lang==='en'?en:pt;
export function applyLanguage(){
 document.documentElement.lang=lang==='en'?'en':'pt-BR';
 document.title=t('SMKSYNTH — Teste SMK-37 Elite','SMKSYNTH — SMK-37 Elite test');
 for(const el of document.querySelectorAll('[data-pt]'))el.textContent=el.dataset[lang];
 for(const el of document.querySelectorAll('[data-language]')){const url=new URL(location.href);url.searchParams.set('lang',el.dataset.language);el.href=url.pathname+url.search;}
 try{localStorage.setItem('smksynth-language',lang)}catch{}
}
