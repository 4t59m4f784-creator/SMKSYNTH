// SPDX-License-Identifier: GPL-3.0-only
// Interface-only fallback when AudioWorklet is unavailable (e.g. insecure preview).
import {Engine} from './engine.js?v=release-120';
let engine,ticks=0,wasm;
onmessage=({data:m})=>{try{if(m.t==='init'){wasm=m.wasm;engine=new Engine(m.wasm,m.store);postMessage({t:'ready',audio:false});setInterval(()=>{try{engine.advance(20);if(++ticks%2===0){const f=engine.frame();postMessage(f,f.fb?[f.fb.buffer]:[]);}if(ticks%50===0){const data=engine.store();if(data)postMessage({t:'store',data},[data]);}}catch(e){postMessage({t:'error',message:e.message});}},20);}else if(m.t==='reboot'){engine=new Engine(wasm,engine.store(true));postMessage({t:'ready',audio:false});}else if(m.t==='flush'){const data=engine.store(true);postMessage({t:'store',data,tag:m.tag},[data]);}else if(engine)engine.input(m);}catch(e){postMessage({t:'error',message:e.message});}};
