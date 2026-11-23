/* SMKSTAGE B3 SMK-37 hardware alpha, GPL-3.0-only.
 * setBfree-fm1 source: R-Jeb, commit 4dd709b, GPL-3.0-only.
 * Never flash an unchecked development build.
 */
#define BOARD_SMK37 1
#define APP_NAME "SMKSTAGE"
#define OBV_DUAL_CORE 1
#include "platform.c"
#include "organ_tab.h"
/* Keep Leslie/reverb delay lines in the large .pool, not 96 KiB system RAM. */
#define ORGAN_POOL __attribute__((section(".pool")))
#define ORGAN_C1_RUN(fn,arg) fm1_mc_run((fn),(arg))
#define ORGAN_C1_WAIT() fm1_mc_wait()
static volatile uint8_t stage_les_bypass;
#include "organ_core.h"
static uint8_t stage_manual, stage_bank, stage_page, stage_f1_caught;
static uint8_t held_manual[37], held_active[37], pad_down[16];
static uint32_t pad_start[16], draw_ms, leds_ms;
static uint8_t favorites[2][8][3][9], saved[2][8], chosen;
static uint8_t full[8][12];
static uint8_t *db(void){return stage_manual==0?(uint8_t*)P.db:stage_manual==1?(uint8_t*)P.dbl:(uint8_t*)P.dbp;}
static uint32_t clamp(int32_t v,uint32_t max){return v<0?0:(uint32_t)v>max?max:(uint32_t)v;}
static void stage_slot_save(uint32_t p){
 if(p>7)return;
 for(uint32_t m=0;m<3;m++)for(uint32_t i=0;i<9;i++)
  favorites[stage_bank][p][m][i]=m==0?P.db[i]:m==1?P.dbl[i]:(i<2?P.dbp[i]:0);
 saved[stage_bank][p]=1;chosen=(uint8_t)p;
 if(stage_bank){full[p][0]=P.perc;full[p][1]=P.perc3;full[p][2]=P.soft;full[p][3]=P.fast;
 full[p][4]=P.vup;full[p][5]=P.vlo;full[p][6]=P.vtype;full[p][7]=P.od;full[p][8]=P.drv;
 full[p][9]=P.rev;full[p][10]=P.swell;full[p][11]=stage_manual;}
}
static void stage_slot_load(uint32_t p){
 if(p>7||!saved[stage_bank][p])return;chosen=(uint8_t)p;
 if(stage_bank){for(uint32_t m=0;m<3;m++)for(uint32_t i=0;i<9;i++){
 uint8_t v=favorites[1][p][m][i];if(m==0)P.db[i]=v;else if(m==1)P.dbl[i]=v;else if(i<2)P.dbp[i]=v;}
 P.perc=full[p][0];P.perc3=full[p][1];P.soft=full[p][2];P.fast=full[p][3];P.vup=full[p][4];P.vlo=full[p][5];
 P.vtype=full[p][6];P.od=full[p][7];P.drv=full[p][8];P.rev=full[p][9];P.swell=full[p][10];stage_manual=full[p][11];
 }else{uint8_t *d=db();for(uint32_t i=0;i<(stage_manual==2?2u:9u);i++)d[i]=favorites[0][p][stage_manual][i];}
 stage_f1_caught=0;
}
static void pad_press(uint32_t p){
 switch(p){
 case 8:stage_les_bypass^=1u;break;  /* pad 9 Leslie on/off */
 case 10:if(!P.perc){P.perc=1;P.perc3=0;}else if(!P.perc3)P.perc3=1;else P.perc=0;break;
 case 11:if(P.perc)P.soft^=1u;break;
 case 12:if(P.perc)P.fast^=1u;break;
 case 13:if(stage_manual==1)P.vlo^=1u;else P.vup^=1u;break;
 case 14:P.vtype=(P.vtype+1u)%6u;break;
 case 15:P.od^=1u;break;
 }}
static void controls(void){
 uint32_t rel=0,b=fm1_input_edges(&rel);
 if(b&(1u<<BTN_LEFT)){stage_manual=(stage_manual+2u)%3u;stage_f1_caught=0;}
 if(b&(1u<<BTN_RIGHT)){stage_manual=(stage_manual+1u)%3u;stage_f1_caught=0;}
 if(b&(1u<<BTN_KNOB_BANK))stage_bank^=1;
 if(b&(1u<<BTN_EDIT))stage_page=(stage_page+1u)%4u;
 if(b&(1u<<BTN_HOME))stage_page=0;
 for(uint32_t i=0;i<8;i++){int32_t v=fm1_enc_take(i);if(!v)continue;
 if(stage_page==0){uint8_t *d=db();if(stage_manual!=2||i<2)d[i]=(uint8_t)clamp((int32_t)d[i]+v,8);}
 else if(stage_page==1)les_set(i,clamp((int32_t)les_p[i]+v*(i<4?5:25),i<4?9000:10000));
 else if(stage_page==2){if(i==0)P.drv=(uint8_t)clamp((int32_t)P.drv+v,20);else if(i==1)P.chr=(uint8_t)clamp((int32_t)P.chr+v,127);}
 else if(stage_page==3){if(i==0)P.vtype=(uint8_t)clamp((int32_t)P.vtype+v,5);}
 }
 static uint8_t last[4];
 for(uint32_t i=0;i<4;i++){uint8_t f=fm1_in.fader[i];if(last[i]==f)continue;last[i]=f;
 if(i==0){uint8_t *d=db(),at=stage_manual==2?1:8,val=(uint8_t)((f*8u+63u)/127u);
  if(!stage_f1_caught){if(val==d[at])stage_f1_caught=1;else continue;} d[at]=val;
 }else if(i==1)P.swell=f;else if(i==2)P.drv=(uint8_t)((f*20u+63u)/127u);else P.rev=f;}
 int32_t event;while((event=smk_pad_take())>=0){
 uint32_t p=((uint32_t)event>>8)&15u,on=(uint32_t)event&255u;
 if(on){pad_down[p]=1;pad_start[p]=fm1_ms;if(p>=8&&p!=9)pad_press(p);}
 else if(pad_down[p]){uint32_t duration=fm1_ms-pad_start[p];pad_down[p]=0;
  if(p==9){if(duration>=500u)P.les=0;else P.les=P.les==0?1:P.les==1?2:1;}
  else if(p<8){if(duration>=600u)stage_slot_save(p);else stage_slot_load(p);}
 }}
}
static void keys_tick(void){
 uint64_t k=fm1_in.keys;
 for(uint32_t i=0;i<37;i++){uint8_t on=(uint8_t)((k>>i)&1u);if(on==held_active[i])continue;
 if(on){held_manual[i]=stage_manual;organ_note(stage_manual,1,48+i,1,1);}
 else organ_note(held_manual[i],1,48+i,0,1);
 held_active[i]=on;}
}
static void leds(void){
 for(uint32_t i=0;i<16;i++){uint32_t r=0,g=0,b=0;
 if(i<8){r=saved[stage_bank][i]?90:8;g=chosen==i?50:0;}
 else switch(i){
 case 8:if(!stage_les_bypass)r=g=b=100;break;
 case 9:if(!stage_les_bypass){uint32_t speed=P.les==2?220u:P.les==1?65u:2u;
 uint32_t phase=(fm1_ms*speed/1000u)%256u,v=phase<128?phase:255-phase;r=g=b=P.les==0?15:18+v/2;}break;
 case 10:if(P.perc){r=P.perc3?20:140;g=P.perc3?30:70;b=P.perc3?140:0;}break;
 case 11:if(P.perc)r=g=b=P.soft?20:100;break;
 case 12:if(P.perc)r=g=b=P.fast?100:20;break;
 case 13:if(stage_manual==1?P.vlo:P.vup)g=b=90;break;
 case 14:if(stage_manual==1?P.vlo:P.vup){b=P.vtype%2u?40u+P.vtype*20u:0;g=P.vtype%2u?0:40u+P.vtype*20u;}break;
 case 15:if(P.od)r=20u+P.drv*8u;break;
 }
 padled_set(i,(uint8_t)r,(uint8_t)g,(uint8_t)b);
 }padled_flush();
}
static void screen_draw(void){
 uint8_t *d=db();uint32_t count=stage_manual==2?2:9;
 lcd_fill(0,0,240,240,C_BLACK);
 draw_text_box(0,2,240,&FONT_S,"SMKSTAGE B3",C_WHITE,1);
 draw_text_box(0,25,240,&FONT_S,stage_manual==0?"UPPER":stage_manual==1?"LOWER":"PEDAL",RGB(240,55,65),1);
 draw_text_box(0,47,240,&FONT_S,stage_bank?"PRESET FULL":"DRAWBARS",C_WHITE,1);
 cv_begin(240,128,C_BLACK);
 for(uint32_t j=0;j<9;j++){int32_t x=4+(int32_t)j*26;uint32_t v=j<count?d[j]:0;
 cv_rect(x,4,19,95,RGB(34,38,45));
 if(v)cv_rect(x,99-(int32_t)v*11,19,(int32_t)v*11,RGB(215,50,65));
 char s[2]={(char)('0'+v),0};
 cv_text(x+5,104,&FONT_S,s,j<count?C_WHITE:RGB(65,65,65));
 }
 cv_blit(0,68);
 draw_text_box(0,201,240,&FONT_S,stage_page==0?"K1-K8 DRAWBARS":stage_page==1?"LESLIE EDIT":stage_page==2?"DRIVE EDIT":"VIBRATO EDIT",C_WHITE,1);
}
static void app_main(void){
 uint8_t st,a,b;smk_pad_events=1;smk_pad_grid=1;palette_set(3);
 organ_program(5u);P.les=1;
 for(;;){platform_poll();keys_tick();controls();
 while(midi_get(&st,&a,&b)){uint32_t ch=st&15u;
 switch(st&0xF0u){case 0x90u:organ_midi_note(ch,a,b!=0);break;case 0x80u:organ_midi_note(ch,a,0);break;
 case 0xB0u:organ_cc_ch(ch,a,b);break;case 0xC0u:organ_pc(a);break;}}
 if(fm1_ms-leds_ms>=40u){leds_ms=fm1_ms;leds();}
 if(fm1_ms-draw_ms>=100u){draw_ms=fm1_ms;screen_draw();}
 }}
