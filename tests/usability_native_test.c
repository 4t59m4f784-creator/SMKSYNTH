#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
int main(void){engine_init();pf_init();pat_init(&pat);seq_trk=0;page=PG_SEQ;rec=REC_ON;ctl_seq_run=1;layer=L_SEQ;layer_lock=1;pf_seq_mode=1;
 fm1_ms=100;fm1_in.buttons=1u<<BTN_KNOB_BANK;pf_bank_poll(fm1_in.buttons,0);fm1_ms=900;pf_bank_poll(0,0);assert(pf_diag&&pf_bank()==0&&rec==REC_ON&&ctl_seq_run&&layer==L_NONE&&!layer_lock&&pf_page_request==PG_PERFORM+1u);fm1_in.buttons=0;pf_bank_poll(0,1u<<BTN_KNOB_BANK);assert(!pf_diag&&pf_bank()==0);
 for(unsigned n=0;n<30;n++){fm1_ms+=200;fm1_in.buttons=1u<<BTN_KNOB_BANK;pf_bank_poll(fm1_in.buttons,0);fm1_ms+=100;fm1_in.buttons=0;pf_bank_poll(0,1u<<BTN_KNOB_BANK);assert(pf_bank()==(n%3==0?3:n%3==1?2:0));}
 /* Every original preset/FX LED follows its new pad, not a stale old location. */
 set.mix_dly[0]=set.mix_rev[0]=127;live_p[0].v[P_DLY_MIX]=live_p[0].v[P_REV_MIX]=127;assert(pf_led(4)!=pf_led(6));
 puts("PASS: long bank from locked SEQ/REC cancels UI layer, retains transport and REC; 30 cycle transitions; remapped LED roles.");}
