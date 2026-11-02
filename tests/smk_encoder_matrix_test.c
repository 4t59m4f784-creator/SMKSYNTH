/* SPDX-License-Identifier: GPL-3.0-only
 * Compare with PF11, using the same actions on each logical control. */
#ifndef OBV_TEST_APP
#define OBV_TEST_APP "../firmware/src/obervave.c"
#endif
#define main obv_host_main
#include OBV_TEST_APP
#undef main
#include <assert.h>
static uint32_t hash(uint32_t h,const void *p,unsigned n){const uint8_t*b=p;while(n--)h=(h^*b++)*16777619u;return h;}
static void reset(unsigned pg,unsigned l,unsigned row,unsigned view,int held){
 engine_init();memset(&set,0,sizeof set);set.tempo=120;set.midi_ch=1;seq_trk=1;
 for(unsigned t=0;t<4;t++)trk_load(t,t+1);
 pat_init(&pat);set.fx_ok=FX_OK;fx_sync();mix_apply();
 page=pg;layer=l;mix_row=row;seq_view=view;step_held=held;entry_n=0;rec=0;save_armed=0;
 slot_sel=0;pat_queue=ctl_seq_run=copy_to=pb_write_target=pend_slot=0;
 memset(host_enc,0,sizeof host_enc);memset(seq_cur,0,sizeof seq_cur);memset(seq_pg,0,sizeof seq_pg);
}
static void scenario(unsigned pg,unsigned l,unsigned row,unsigned view,int held,unsigned control){
 reset(pg,l,row,view,held);
 unsigned physical=control<4?ENC_KNOB1+control:control==4?ENC_SELECT:control==5?ENC_ALGORITHM:ENC_PRESETS;
 host_enc[physical]=1;panel_encoders();
 uint32_t h=2166136261u;
 h=hash(h,&set,sizeof set);h=hash(h,live_p,sizeof live_p);
#ifdef DRUM_SEQ_MAGIC
 h=hash(h,&pat,PAT_LEGACY_BYTES); /* compare the exact PF12 region; extension has separate tests */
#else
 h=hash(h,&pat,sizeof pat);
#endif
 h=hash(h,seq_cur,sizeof seq_cur);h=hash(h,seq_pg,sizeof seq_pg);h=hash(h,&seq_trk,sizeof seq_trk);
 h=hash(h,&slot_sel,sizeof slot_sel);h=hash(h,&copy_to,sizeof copy_to);
 printf("MATRIX %u %u %u %u %d %u %08x\n",pg,l,row,view,held,control,h);
}
int main(void){
#ifndef OBV_REFERENCE
 assert(ENC_KNOB1==0&&ENC_KNOB2==1&&ENC_KNOB3==2&&ENC_KNOB4==3);
 assert(ENC_SELECT==4&&ENC_ALGORITHM==5&&ENC_PRESETS==6&&ENC_MASTER==7);
 assert(!strcmp(NM_SELECT,"K5")&&!strcmp(NM_ALGO,"K6")&&!strcmp(NM_PRESETS,"K7"));
#endif
 for(unsigned pg=0;pg<NPAGE;pg++)for(unsigned c=0;c<7;c++)scenario(pg,L_NONE,0,SV_TRACKS,-1,c);
 for(unsigned row=0;row<7;row++)for(unsigned c=0;c<7;c++)scenario(PG_MIXER,L_NONE,row,0,-1,c);
 for(unsigned view=0;view<NSV;view++)for(unsigned c=0;c<7;c++)scenario(PG_SEQ,L_NONE,0,view,-1,c);
 for(unsigned pg=0;pg<NPAGE;pg++)for(unsigned l=1;l<=L_FX;l++)for(unsigned c=0;c<4;c++)scenario(pg,l,0,SV_STEPS,0,c);
 reset(PG_PERFORM,L_NONE,0,0,-1);save_armed=1;save_to=3;host_enc[ENC_PRESETS]=1;panel_encoders();assert(save_to==4&&save_armed);
 return 0;
}
