// SPDX-License-Identifier: GPL-3.0-only
// Input only: no MIDI outputs, SysEx, program changes or external transport.
export class MidiInputController {
  constructor({navigator,secureContext,send,isRunning,isListening=()=>true,onPorts=()=>{},onStatus=()=>{},onNote=()=>{},onWheel=()=>{}}){
    Object.assign(this,{navigator,secureContext,send,isRunning,isListening,onPorts,onStatus,onNote,onWheel});
    this.access=null;this.input=null;this.selectedId='';this.generation=0;this.notes=new Map();this.request=null;
  }
  async connect(){
    if(!this.secureContext||!this.navigator.requestMIDIAccess){this.onStatus('unsupported');return;}
    if(this.request)return this.request;
    this.onStatus('permission');
    const task=(async()=>{try{
      if(!this.access){this.access=await this.navigator.requestMIDIAccess({sysex:false});this.access.onstatechange=()=>this.refresh();}
      this.refresh();
      const ports=this.ports();
      if(!this.selectedId&&ports.length===1)await this.select(ports[0].id);
      else if(!this.selectedId)this.onStatus(ports.length?'choose':'empty');
    }catch(e){this.onStatus(e.name==='NotAllowedError'||e.name==='SecurityError'?'denied':'error');}
    })();
    this.request=task;
    try{return await task;}finally{this.request=null;}
  }
  ports(){return this.access?[...this.access.inputs.values()].filter(p=>p.state==='connected'):[];}
  refresh(){
    const ports=this.ports();
    if(this.selectedId&&!ports.some(p=>p.id===this.selectedId)){
      void this.select('','disconnected');
    }
    this.onPorts(ports.map(p=>({id:p.id,name:p.name||p.manufacturer||'MIDI'})),this.selectedId);
  }
  async select(id,offStatus='off'){
    const serial=++this.generation,old=this.input;
    this.input=null;this.selectedId=id;
    if(old){old.onmidimessage=null;this.release();}
    this.onPorts(this.ports().map(p=>({id:p.id,name:p.name||p.manufacturer||'MIDI'})),id);
    try{
      if(old)await old.close();
      if(serial!==this.generation)return;
      if(!id){this.onStatus(offStatus);return;}
      const input=this.access?.inputs.get(id);
      if(!input||input.state!=='connected')throw Error('disconnected');
      this.onStatus('opening');await input.open();
      if(serial!==this.generation){if(this.selectedId!==id)await input.close();return;}
      if(input.state!=='connected')throw Error('disconnected');
      this.input=input;
      input.onmidimessage=event=>{if(this.input===input)this.message(event.data);};
      this.onStatus('connected',input.name||input.manufacturer||'MIDI');
    }catch{if(serial===this.generation){this.input=null;this.selectedId='';this.refresh();this.onStatus('error');}}
  }
  message(data){
    if(!this.isRunning()||!this.isListening()||!data||data.length!==3)return;
    const [st,d1,d2]=data,kind=st&0xf0,ch=st&15;
    if(st<0x80||st>=0xf0||d1>127||d2>127)return;
    if(kind===0x90||kind===0x80){
      const id=ch*128+d1;
      if(kind===0x90&&d2)this.notes.set(id,d1);else this.notes.delete(id);
      this.onNote(d1,[...this.notes.values()].includes(d1));
      this.send({t:'midi',st:kind===0x90&&d2?st:0x80|ch,d1,d2:kind===0x90&&d2?d2:0});
    }else if(kind===0xe0){this.send({t:'midi',st,d1,d2});this.onWheel('pitch',d1|(d2<<7));}
    else if(kind===0xb0&&d1===1){
      // The firmware reads modulation from the SMK wheel ADC, rather than CC1.
      const raw=620-Math.round(d2*320/127);this.send({t:'wheel',w:1,v:raw});this.onWheel('mod',raw);
    }else if(kind===0xb0&&[64,120,123].includes(d1)){
      this.send({t:'midi',st,d1,d2});
      if(d1!==64)for(const [id,note]of this.notes)if((id>>7)===ch){this.notes.delete(id);this.onNote(note,[...this.notes.values()].includes(note));}
    }
  }
  release(){
    for(const note of new Set(this.notes.values()))this.onNote(note,false);this.notes.clear();
    if(!this.isRunning())return;
    for(let ch=0;ch<16;ch++){
      this.send({t:'midi',st:0xb0|ch,d1:64,d2:0});
      this.send({t:'midi',st:0xe0|ch,d1:0,d2:64});
    }
    this.send({t:'wheel',w:1,v:620});this.send({t:'panic'});this.onWheel('pitch',8192);this.onWheel('mod',620);
  }
}
