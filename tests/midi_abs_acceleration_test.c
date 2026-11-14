/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void tick(unsigned ms,unsigned knob,int d){fm1_ms+=ms;host_enc[knob]=d;mm_input();}
int main(void){
 mm_boot();fm1_ms=1000;
 assert(mm_abs_delta(0,1)==1);fm1_ms+=150;assert(mm_abs_delta(0,1)==1);
 fm1_ms+=70;assert(mm_abs_delta(0,1)==2);fm1_ms+=35;assert(mm_abs_delta(0,1)==4);fm1_ms+=15;assert(mm_abs_delta(0,1)==8);
 assert(mm_abs_delta(0,-1)==-1);fm1_ms+=15;assert(mm_abs_delta(0,-1)==-8);
 assert(mm_abs_delta(1,1)==1); /* another physical knob never borrows acceleration */
 fm1_ms+=500;assert(mm_abs_delta(0,-1)==-1);mm_abs_reset();assert(mm_abs_delta(0,4)==16);assert(mm_abs_delta(0,-4)==-4);
 /* Actual panel: slow precision, fast sweep, reversal, clamp and both banks. */
 mm_on=1;mm_edit=mm_settings=mm_reset_ask=mm_panic_left=0;mm_kbank=0;mm_abs_reset();memset(mm_values,0,sizeof mm_values);
 for(unsigned i=0;i<4;i++)tick(150,0,1);assert(mm_values[0]==4);
 for(unsigned i=0;i<16;i++)tick(15,0,1);assert(mm_values[0]==127);tick(15,0,1);assert(mm_values[0]==127);
 tick(15,0,-1);assert(mm_values[0]==126);for(unsigned i=0;i<16;i++)tick(15,0,-1);assert(mm_values[0]==0);
 mm_kbank=1;mm_abs_reset();tick(15,7,1);assert(mm_values[15]==1);tick(15,7,1);assert(mm_values[15]==9);
 /* REL packets remain exact binary offset; editor numbers retain single steps. */
 mm_profile()->relative=1;memset(mm_cv_delta,0,sizeof mm_cv_delta);tick(1,0,1);tick(1,0,1);assert(mm_cv_delta[8]==2);
 mm_edit=1;mm_pick=0;mm_sel=MM_PAD;unsigned num=mm_profile()->map[MM_PAD].num;tick(1,1,1);assert(mm_profile()->map[MM_PAD].num==num+1);tick(1,1,1);assert(mm_profile()->map[MM_PAD].num==num+2);
 puts("PASS: slow 1, medium 2, fast 4/8; fast 0-127 sweep, bounded at both ends, reverse fine step, saturation, independent knobs/banks; REL and editor unchanged.");
}
