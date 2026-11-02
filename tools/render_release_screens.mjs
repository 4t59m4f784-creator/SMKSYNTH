import fs from 'node:fs';
const lab=new URL('../web/simulator/',import.meta.url);const {Engine}=await import('data:text/javascript;base64,'+fs.readFileSync(new URL('engine.js',lab)).toString('base64'));
const s=fs.readFileSync(new URL('factory-session.bin',lab)),w=fs.readFileSync(new URL('obervave-voice.wasm',lab));const e=new Engine(w,s.buffer.slice(s.byteOffset,s.byteOffset+s.byteLength));e.advance(5000);
const b=n=>{e.input({t:'btn',b:n,down:true});e.advance(100);e.input({t:'btn',b:n,down:false});e.advance(150);};
function screen(name){const a=new Uint16Array(e.ex.memory.buffer,e.ex.sim_fb(),57600),rgb=Buffer.alloc(57600*3);for(let i=0;i<a.length;i++){rgb[3*i]=Math.round((a[i]>>11)*255/31);rgb[3*i+1]=Math.round(((a[i]>>5)&63)*255/63);rgb[3*i+2]=Math.round((a[i]&31)*255/31);}fs.writeFileSync(new URL('../build/'+name+'.ppm',import.meta.url),Buffer.concat([Buffer.from('P6\n240 240\n255\n'),rgb]));}
screen('performance');b(15);b(16);e.advance(200);screen('sequence');b(16);
for(let i=0;i<3;i++){e.input({t:'enc',e:5,steps:1});e.advance(100);}
e.input({t:'drums',part:3,kit:0});e.advance(300);for(let i=0;i<8&&e.frame().bank!==4;i++)b(0);if(e.frame().bank!==4)throw Error('Drum bank not reached');e.advance(300);screen('drum-pads');
console.log('Rendered actual firmware screens, seeded factory session.');
