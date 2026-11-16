/* SPDX-License-Identifier: GPL-3.0-only */
/* GLOBE + SCALE opens diagnostics. No persistent settings or new page IDs. */
static void dc_diag_line(unsigned y,const char *name,uint32_t v)
{
    char s[48];s[0]=0;scat(s,name);fmt_u(s+slen(s),v);
    draw_text_box(6,y,228,&FONT_S,s,C_WHITE,0);
}
static int dc_diag_poll(uint32_t b,uint32_t rel)
{
    static uint8_t open,ready;
    static uint32_t shown;
    uint32_t held=fm1_in.buttons;
    (void)rel;
    if(!open){
        uint32_t chord=(1u<<BTN_GLO)|(1u<<BTN_SEL);
        if((held&chord)!=chord||!(b&chord))return 0;
        open=1;ready=0;shown=0;layer=L_NONE;layer_lock=0;layer_used=1;
    }
    if(!ready){if(!(held&((1u<<BTN_GLO)|(1u<<BTN_SEL))))ready=1;}
    else if(b&(1u<<BTN_HOME)){open=0;dirty_page=dirty_hdr=1;return 1;}
    for(unsigned e=0;e<8;e++){
        int d=fm1_enc_take(e);if(!ready||!d)continue;
        if(e==0){dc_enabled=d>0;shown=0;}
        if(e==1){
            fm1_irq_off();dc_jobs=dc_fallbacks=dc_worker_peak_ticks=dc_wait_peak_ticks=0;
            dc_irq_peak_ticks=dc_irq_overruns=dc_clips=ui_render_peak_us=ui_render_overruns=0;fm1_irq_on();shown=0;
        }
    }
    if(shown&&fm1_ms-shown<500u)return 1;
    shown=fm1_ms?fm1_ms:1;
    lcd_fill(0,0,240,240,C_BLACK);
    draw_text_box(0,4,240,&FONT_S,"DC1 AUDIO DIAGNOSTICS",C_WHITE,1);
    draw_text_box(0,26,240,&FONT_S,!fm1_mc_on?"CPU1 UNAVAILABLE / 1C":dc_enabled?"2C / DRUM-X ON CPU1":"1C / ALL AUDIO ON CPU0",C_WHITE,1);
    dc_diag_line(50,"IRQ PEAK us ",dc_irq_peak_ticks/FM1_TICKS_PER_US);
    dc_diag_line(70,"IRQ OVER ",dc_irq_overruns);
    dc_diag_line(90,"CLIP SAMPLES ",dc_clips);
    dc_diag_line(110,"CPU1 JOBS ",dc_jobs);
    dc_diag_line(130,"CPU1 PEAK us ",dc_worker_peak_ticks/FM1_TICKS_PER_US);
    dc_diag_line(150,"WAIT PEAK us ",dc_wait_peak_ticks/FM1_TICKS_PER_US);
    dc_diag_line(170,"FALLBACKS ",dc_fallbacks);
    draw_text_box(0,195,240,&FONT_S,"K1: 1C < / > 2C",C_WHITE,1);
    draw_text_box(0,215,240,&FONT_S,"K2: RESET  PATCH: EXIT",C_WHITE,1);
    return 1;
}
