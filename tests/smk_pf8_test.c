/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <stdio.h>
#define CHECK(c) do { if(!(c)){printf("FAIL line %d: %s\n",__LINE__,#c);return 1;}checks++; } while(0)
int main(void){unsigned checks=0;int32_t q=535*256;
 for(int raw=535;raw<=635;raw++){int out=0;for(int i=0;i<40;i++)out=smk_wheel_filter(&q,raw);CHECK(out==raw);}
 for(int raw=635;raw>=435;raw--){int out=0;for(int i=0;i<40;i++)out=smk_wheel_filter(&q,raw);CHECK(out==raw);}
 q=0;int prev=0;for(int i=0;i<200;i++){int out=smk_bend_slew(&q,512);CHECK(out>=prev&&out<=512);prev=out;}CHECK(prev==512);
 prev=512;for(int i=0;i<200;i++){int out=smk_bend_slew(&q,-512);CHECK(out<=prev&&out>=-512);prev=out;}CHECK(prev==-512);
 for(int i=0;i<200;i++)smk_bend_slew(&q,0);CHECK(q==0);
 set.rec_q=3;smk_master=12;smk_boot_volume_restore();CHECK(smk_master==724);
 for(int v=0;v<=1023;v++){smk_master=v;smk_boot_volume_capture();CHECK(set.rec_q&128);smk_master=0;smk_boot_volume_restore();CHECK(abs(smk_master-v)<=4);}
 smk_master=880;smk_boot_volume_capture();settings_save(&set,sizeof set);smk_master=200;settings_load(&set,sizeof set);smk_boot_volume_restore();CHECK(abs(smk_master-880)<=4);
 for(int i=0;i<4;i++)host_write_count[i]=0;
 smk_master=320;for(int i=0;i<500;i++){q=0;smk_wheel_filter(&q,i);smk_bend_slew(&q,512);}
 for(int i=0;i<4;i++)CHECK(!host_write_count[i]);
 printf("PF8: %u checks passed: 1-count slow sweeps both ways, monotonic bend +/-2, centre return, boot-only volume/default/precision, no control writes.\n",checks);return 0;}
