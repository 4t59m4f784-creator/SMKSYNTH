/* SPDX-License-Identifier: GPL-3.0-only
 * Regression tests using the real key-contact state machine, without MMIO. */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "smk_input.h"
#ifndef SMK_KEY_TMAX_MS
#define SMK_KEY_TMAX_MS 40u
#endif
static unsigned fails, checks, on_count, off_count, last_vel;
static uint64_t prev;
#define CHECK(c, label) do { checks++; if (!(c)) { fails++; printf("FAIL key %u: %s\n", key, label); } } while (0)
static void contacts(unsigned k, unsigned a, unsigned b) {
    unsigned values[2]={a,b};
    for(unsigned i=0;i<2;i++) {unsigned code=SMK_KEYC[k][i];
        if(values[i])fm1_in.raw[code>>4]|=(uint16_t)(1u<<(code&15));
        else fm1_in.raw[code>>4]&=(uint16_t)~(1u<<(code&15));}
}
static void scan(uint32_t start, unsigned us) {
    smk__keys(start+(uint32_t)us*FM1_TICKS_PER_US);
    uint64_t changed=fm1_in.keys^prev;
    if(changed&fm1_in.keys) {on_count++; for(unsigned i=0;i<37;i++)if(changed>>i&1)last_vel=fm1_in.key_vel[i];}
    if(changed&prev)off_count++;
    prev=fm1_in.keys;
}
static void reset(void) {memset((void *)&fm1_in,0,sizeof fm1_in);prev=0;on_count=off_count=last_vel=0;smk_extq_w=smk_extq_r=0;smk_trim_defaults();}
static void release(unsigned key, uint32_t start, unsigned us) {
    contacts(key,0,0);scan(start,us);scan(start,us+1600);scan(start,us+3200);
    CHECK(on_count==1&&off_count==1&&!fm1_in.keys,"exactly one note-on/off");
    if(key<5||key>=32) {int32_t a=smk_extkey_take(),b=smk_extkey_take();
        CHECK(a>=0&&(unsigned)a>>8==key&&(a&255)>0,"outer note-on queued");
        CHECK(b==(int32_t)(key<<8)&&smk_extkey_take()==-1,"outer note-off queued, no duplicate");}
}
int main(void) {
    for(unsigned key=0;key<37;key++) {
        reset();contacts(key,1,0);scan(0,0);contacts(key,1,1);scan(0,2000);
        CHECK(on_count==1&&last_vel>=120,"fast strike remains immediate and loud");release(key,0,10000);
        reset();contacts(key,1,0);scan(0,0);contacts(key,1,1);scan(0,15000);
        CHECK(on_count==1&&last_vel>=60&&last_vel<=100,"medium strike keeps velocity");release(key,0,20000);
        for(unsigned wrap=0;wrap<2;wrap++) {
            uint32_t start=wrap?UINT32_MAX-20000u*FM1_TICKS_PER_US:0;
            reset();contacts(key,1,0);scan(start,0);scan(start,39000);CHECK(on_count==0,"no early slow strike");
            scan(start,40000);CHECK(on_count==1&&last_vel<=40,"one soft note at timeout, including timer wrap");
            scan(start,80000);contacts(key,1,1);scan(start,100000);CHECK(on_count==1,"late second contact does not retrigger");
            contacts(key,1,0);scan(start,101600);CHECK(off_count==0,"opening only second contact does not release");
            contacts(key,0,0);scan(start,103200);contacts(key,1,0);scan(start,104800);
            CHECK(off_count==0,"single-frame open bounce does not release");release(key,start,106400);
        }
        reset();contacts(key,1,1);scan(0,0);CHECK(on_count==1,"both contacts at once");release(key,0,10000);
        reset();contacts(key,0,1);scan(0,0);CHECK(on_count==1,"second contact alone is normalized");release(key,0,10000);
    }
    /* Actual HAL FIFO: consume a 30-event burst (15 on/off pairs), below its capacity of 31. */
    reset();for(unsigned i=0;i<15;i++){smk__key_event(0,90);smk__key_event(0,0);}
    for(unsigned i=0;i<30;i++){int32_t ev=smk_extkey_take();checks++;if(ev!=(int32_t)(i%2?0:90))fails++;}
    checks++;if(smk_extkey_take()!=-1)fails++;
    printf("CONTACTS: %u checks, %u failures; all 37 keys, timer wrap, bounce, real HAL FIFO\n",checks,fails);
    return fails?1:0;
}
