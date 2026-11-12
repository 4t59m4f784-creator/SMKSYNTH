/* SPDX-License-Identifier: GPL-3.0-only */
/* Main-loop producer, 10 kHz timer consumer. Never waits for a UART byte. */
#define SMK_TX_BYTES 512u
static uint8_t smk_tx_bytes[SMK_TX_BYTES];
static volatile uint32_t smk_tx_w, smk_tx_r;
static unsigned smk_tx_busy;
static int smk_tx_ready(void) { return smk_tx_w-smk_tx_r <= SMK_TX_BYTES-3u; }
static void smk_tx_message(unsigned st,unsigned a,unsigned b)
{
    unsigned w=smk_tx_w;
    smk_tx_bytes[w++%SMK_TX_BYTES]=(uint8_t)st;
    smk_tx_bytes[w++%SMK_TX_BYTES]=(uint8_t)(a&127u);
    if((st&0xE0u)!=0xC0u)smk_tx_bytes[w++%SMK_TX_BYTES]=(uint8_t)(b&127u);
    RING_PUBLISH();smk_tx_w=w;
}
static void smk_tx_poll(void)
{
    if(smk_tx_busy&&!smk_uart1_tx_done())return;
    if(smk_tx_r==smk_tx_w){smk_tx_busy=0;return;}
    smk_uart1_tx_put(smk_tx_bytes[smk_tx_r++%SMK_TX_BYTES]);smk_tx_busy=1;
}
