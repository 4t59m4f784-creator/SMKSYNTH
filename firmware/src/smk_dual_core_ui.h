/* SPDX-License-Identifier: GPL-3.0-only */
/* DC3: K2 arms a fixed transport-started window. K3 chooses frozen results. */
static void dc_diag_line(unsigned y,const char *name,uint32_t v)
{
    char s[48];s[0]=0;scat(s,name);fmt_u(s+slen(s),v);
    draw_text_box(6,y,228,&FONT_S,s,C_WHITE,0);
}
static void dc_diag_metric(unsigned y,const char *name,unsigned k)
{
    char s[48];s[0]=0;scat(s,name);scat(s," ");
    fmt_u(s+slen(s),dp_halves?dp_sum[k]/dp_halves/FM1_TICKS_PER_US:0);
    scat(s," / ");fmt_u(s+slen(s),dp_peak[k]/FM1_TICKS_PER_US);
    draw_text_box(6,y,228,&FONT_S,s,C_WHITE,0);
}
static int dc_diag_poll(uint32_t b,uint32_t rel)
{
    static uint8_t open,ready,view,record_mode;
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
        if(e==0&&dp_state!=DP_ARMED&&dp_state!=DP_RUNNING){dc_enabled=d>0;shown=0;}
        if(e==1&& !ctl_seq_run){
            fm1_irq_off();record_mode=dc_enabled;dp_arm();dc_jobs=dc_fallbacks=dc_worker_peak_ticks=dc_wait_peak_ticks=0;
            dc_irq_peak_ticks=dc_irq_overruns=dc_clips=ui_render_peak_us=ui_render_overruns=0;fm1_irq_on();shown=0;
        }
        if(e==2){view=(uint8_t)((view+(d>0?1:2))%3u);shown=0;}
    }
    if(shown&&fm1_ms-shown<500u)return 1;
    shown=fm1_ms?fm1_ms:1;
    lcd_fill(0,0,240,240,C_BLACK);
    char title[40];title[0]=0;scat(title,"DC3 ");
    scat(title,dp_state==DP_ARMED?record_mode?"ARMED 2C PATCH > PLAY":"ARMED 1C PATCH > PLAY":dp_state==DP_RUNNING?"RECORDING":dp_state==DP_DONE?"FROZEN":"STOP PLAY / K2 ARM");
    if(dp_state==DP_DONE)scat(title,record_mode?" 2C":" 1C");
    draw_text_box(0,3,240,&FONT_S,title,C_WHITE,1);
    title[0]=0;scat(title,!fm1_mc_on?"CPU1 OFF":dc_enabled?"NEXT 2C":"NEXT 1C");
    scat(title,"  ");fmt_u(title+slen(title),dp_elapsed/DP_HZ);scat(title,"s  PAGE ");fmt_u(title+slen(title),view+1);
    draw_text_box(0,23,240,&FONT_S,title,C_WHITE,1);
    if(!view){
        dc_diag_line(47,"IRQ PEAK us ",dc_irq_peak_ticks/FM1_TICKS_PER_US);
        dc_diag_line(67,"IRQ OVER ",dc_irq_overruns);
        dc_diag_line(87,"CLIP SAMPLES ",dc_clips);
        dc_diag_line(107,"CPU1 JOBS ",dc_jobs);
        dc_diag_line(127,"CPU1 PEAK us ",dc_worker_peak_ticks/FM1_TICKS_PER_US);
        dc_diag_line(147,"WAIT PEAK us ",dc_wait_peak_ticks/FM1_TICKS_PER_US);
        dc_diag_line(167,"FALLBACKS ",dc_fallbacks);
    }else{
        draw_text_box(0,45,240,&FONT_S,"us/HALF: AVG / PEAK",C_WHITE,1);
        if(view==1){
            dc_diag_metric(67,"ANALOG",DP_ANALOG);dc_diag_metric(87,"SUPER",DP_SUPER);
            dc_diag_metric(107,"DRUM CPU0",DP_DRUM);dc_diag_metric(127,"FX+MASTER",DP_FX);
            dc_diag_metric(147,"MIX",DP_MIX);dc_diag_metric(167,"CTRL+SEQ",DP_CTRL);
        }else{
            dc_diag_metric(67,"COPY",DP_COPY);dc_diag_metric(87,"WAIT",DP_WAIT);
            dc_diag_metric(107,"SCAN+USB IRQ",DP_NEST);dc_diag_metric(127,"IO+OTHER",DP_IO);
            dc_diag_line(147,"AUDIO HALVES ",dp_halves);
            dc_diag_line(167,"BUDGET us ",HALF_FRAMES*544u/FM1_TICKS_PER_US);
        }
    }
    draw_text_box(0,194,240,&FONT_S,"K1:1C/2C  K2:ARM",C_WHITE,1);
    draw_text_box(0,215,240,&FONT_S,"K3:PAGE   PATCH:EXIT",C_WHITE,1);
    return 1;
}
