// SPDX-License-Identifier: GPL-3.0-only
let saved;try{saved=globalThis.localStorage?.getItem('smksynth-language')}catch{}
export const lang=(new URLSearchParams(globalThis.location?.search||'').get('lang')||saved)==='en'?'en':'pt';
export const t=(pt,en)=>lang==='en'?en:pt;
export function applyLanguage(){document.documentElement.lang=lang==='en'?'en':'pt-BR';for(const el of document.querySelectorAll('[data-pt]'))el.textContent=el.dataset[lang];}
