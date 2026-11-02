/* SPDX-License-Identifier: GPL-3.0-only
 * Execute the real app_idle and note path, not a copy of its logic. No physical timing claims. */
#ifndef OBV_TEST_APP
#define OBV_TEST_APP "../firmware/src/obervave.c"
#endif
#define main obv_host_main
#include OBV_TEST_APP
#undef main
#include <assert.h>
static unsigned checks;
#define CHECK(c) do {checks++;if(!(c)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
static void reset_test(void) {
    memset(&set,0,sizeof set);memset(&pat,0,sizeof pat);memset(live_p,0,sizeof live_p);
    memset(smk_ext_note,0,sizeof smk_ext_note);memset(smk_ext_erase,0,sizeof smk_ext_erase);
    memset((void*)ctl_live_hold,0,sizeof ctl_live_hold);memset(note_part,0,sizeof note_part);
    host_qw[0]=host_qw[1]=host_qr[0]=host_qr[1]=0;nq_w=nq_r=0;keys_prev=fm1_in.notes=0;
    idle_ok=1;layer=0;rec=0;step_held=-1;page=PG_PERFORM;seq_trk=0;pen_n=notes_down=0;
    layer_used=0;undo_ok=0;set.midi_ch=2;
}
int main(void) {
    const unsigned outer[]={0,1,2,3,4,32,33,34,35,36};
    for(unsigned t=0;t<4;t++)for(unsigned i=0;i<10;i++) {
        reset_test();seq_trk=t;unsigned key=outer[i],note=48+key;
        host_qput(1,(int32_t)(key<<8|87));app_idle();
        CHECK(nq_w==1);CHECK(nq[0].type==1&&nq[0].note==note&&nq[0].vel==87&&nq[0].part==t);
        CHECK(host_qr[1]==host_qw[1]);
        ctl_live_hold[t][note>>5&3]|=1u<<(note&31);
        seq_trk=(t+1)%4;host_qput(1,(int32_t)(key<<8));app_idle();
        CHECK(nq_w==2);CHECK(nq[1].type==0&&nq[1].note==note&&nq[1].part==t);
        CHECK(!(ctl_live_hold[t][note>>5&3]&(1u<<(note&31))));
        CHECK(smk_ext_note[key]==0);
    }
    reset_test();
    for(unsigned i=0;i<10;i++)host_qput(1,(int32_t)(outer[i]<<8|91));
    fm1_in.notes=1u<<7;fm1_in.key_vel[12]=80;app_idle();
    CHECK(nq_w==11);CHECK(nq[0].note==60);
    for(unsigned i=0;i<10;i++)CHECK(nq[i+1].note==48+outer[i]);
    fm1_in.notes=0;for(unsigned i=0;i<10;i++)host_qput(1,(int32_t)(outer[i]<<8));app_idle();CHECK(nq_w==22);
    reset_test();
    for(unsigned i=0;i<15;i++){host_qput(1,90);host_qput(1,0);}app_idle();
    CHECK(nq_w==30);for(unsigned i=0;i<30;i++)CHECK(nq[i].type==(i%2?0:1));
    /* EDIT and all later events wait in FIFO order until the main loop. */
    reset_test();layer=L_EDIT;host_qput(1,90);host_qput(1,0);host_qput(1,1<<8|80);host_qput(1,1<<8);
    app_idle();CHECK(nq_w==0);CHECK(!layer_used&&!undo_ok);CHECK(host_qr[1]==1);
    smk_pads(KEY0,&dirty_steps);CHECK(layer_used&&undo_ok);CHECK(host_qr[1]==4);CHECK(nq_w==0);
    layer=0;host_qput(1,90);app_idle();CHECK(nq_w==1);
    /* Pads are not drained during draw, so editing is still owned by the main loop. */
    host_qput(0,0<<8|90);app_idle();CHECK(host_qr[0]==0);
    printf("DISPATCH: %u checks passed; idle delivery, 4 tracks, release routing, EDIT deferral, bursts\n",checks);
    return 0;
}
