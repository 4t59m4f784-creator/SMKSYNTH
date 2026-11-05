// SPDX-License-Identifier: GPL-3.0-only
import {Engine} from './engine.js?v=midi-1';
class OberVave extends AudioWorkletProcessor {
 constructor(options){super();this.lastFrame=0;this.lastStore=0;this.wasm=options.processorOptions.wasm;try{this.engine=new Engine(options.processorOptions.wasm,options.processorOptions.store);this.port.postMessage({t:'ready',audio:true});}catch(e){this.port.postMessage({t:'error',message:e.message});}this.port.onmessage=({data:m})=>{if(!this.engine)return;if(m.t==='reboot'){const store=this.engine.store(true);this.engine=new Engine(this.wasm,store);this.port.postMessage({t:'ready',audio:true});}else if(m.t==='flush')this.flush(m.tag);else this.engine.input(m);};}
 flush(tag){const data=this.engine.store(true);this.port.postMessage({t:'store',data,tag},[data]);}
 process(_inputs,outputs){if(!this.engine)return true;try{const x=this.engine.ex,out=outputs[0],n=out[0].length;this.engine.render(n);out[0].set(new Float32Array(x.memory.buffer,x.sim_out_l(),n));out[1].set(new Float32Array(x.memory.buffer,x.sim_out_r(),n));if(currentTime-this.lastFrame>=1/25){this.lastFrame=currentTime;const m=this.engine.frame();this.port.postMessage(m,m.fb?[m.fb.buffer]:[]);}if(currentTime-this.lastStore>=1){this.lastStore=currentTime;const data=this.engine.store();if(data)this.port.postMessage({t:'store',data},[data]);}}catch(e){this.port.postMessage({t:'error',message:e.message});this.engine=null;}return true;}
}
registerProcessor('obervave-smk37',OberVave);
