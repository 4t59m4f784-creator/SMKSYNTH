/* SPDX-License-Identifier: GPL-3.0-only */
/* Musical events: audio ISR produces, main loop drains. The wire ledger holds
 * the ORIGINAL note/channel until off, independently of focus and base edits.
 * Three independent sources share a pitch without releasing each other. */
#ifndef RING_PUBLISH
#define RING_PUBLISH() __asm__ volatile("" ::: "memory")
#endif
/* Keep raw MIDI input out of the output path; generated arpeggios still transmit. */
enum { MOUT_KEY, MOUT_ARP, MOUT_SEQ, MOUT_SOURCES };
#define MOUT_Q 512u
static uint8_t mout_q[MOUT_Q][3];
static volatile uint32_t mout_w,mout_r,mout_fault,mout_blocked;
static volatile uint32_t mout_used_channels, mout_epoch;
static volatile uint32_t mout_base; /* zero based, 0..12; OMNI receives, TX 1..4 */
static uint16_t mout_owner[MOUT_SOURCES][4][128];
static uint8_t mout_refs[16][128];
static uint8_t mout_seq_vel[4][128];
static uint32_t mout_drum_due[4][8];
static uint8_t mout_drum_note[4][8];
static unsigned midi_input_active,mout_suppress;
static unsigned mout_channel(unsigned p){return (mout_base>12u?12u:mout_base)+(p%4u);}
static void mout_emit(unsigned st,unsigned a,unsigned b)
{
    if(mout_blocked)return;
    if(mout_w-mout_r>=MOUT_Q){mout_fault=mout_blocked=1;return;}
    unsigned w=mout_w%MOUT_Q;mout_q[w][0]=st;mout_q[w][1]=a;mout_q[w][2]=b;
    RING_PUBLISH();mout_w++;
}
static void mout_off(unsigned p,unsigned n,unsigned source)
{
    uint16_t wire=mout_owner[source][p][n];
    if(!wire)return;
    mout_owner[source][p][n]=0;
    unsigned ch=(wire>>7)&15u,pitch=wire&127u;
    if(mout_refs[ch][pitch]&&!--mout_refs[ch][pitch])mout_emit(0x80u|ch,pitch,0);
}
static void mout_on(unsigned p,unsigned n,unsigned vel,unsigned source,int transpose)
{
    if(mout_blocked||mout_suppress)return;
    uint16_t wire=(uint16_t)(0x8000u|(mout_channel(p)<<7)|(unsigned)clampi((int)n+transpose,0,127));
    if(mout_owner[source][p][n])mout_off(p,n,source);
    unsigned ch=(wire>>7)&15u,pitch=wire&127u;
    if(mout_refs[ch][pitch]==255u){mout_fault=mout_blocked=1;return;}
    unsigned shared=mout_refs[ch][pitch]++;
    mout_used_channels|=1u<<ch;
    mout_owner[source][p][n]=wire;
    if(source==MOUT_SEQ)mout_seq_vel[p][n]=(uint8_t)vel;
    if(!shared)mout_emit(0x90u|((wire>>7)&15u),wire&127u,vel);
}
static void mout_part_off(unsigned p)
{
    for(unsigned s=0;s<MOUT_SOURCES;s++)for(unsigned n=0;n<128;n++)mout_off(p,n,s);
    memset(mout_drum_due[p],0,sizeof mout_drum_due[p]);
}
static void mout_clear(void) /* main, interrupts disabled */
{
    mout_epoch++;mout_r=mout_w;memset(mout_owner,0,sizeof mout_owner);memset(mout_refs,0,sizeof mout_refs);
    memset(mout_drum_due,0,sizeof mout_drum_due);mout_fault=0;
}
static void mout_poll(void)
{
    /* Bounded work per yield; UART throughput, not synth CPU, limits DIN. */
    for(unsigned i=0;i<16&&mout_r!=mout_w&&midi_send_ready();i++){
        unsigned r=mout_r%MOUT_Q;
        midi_send(mout_q[r][0],mout_q[r][1],mout_q[r][2]);mout_r++;
    }
}
