/* SMKSTAGE B3 SMK-37 hardware alpha, GPL-3.0-only.
 * setBfree-fm1 source: R-Jeb, commit 4dd709b, GPL-3.0-only.
 * Never flash an unchecked development build.
 */
#define BOARD_SMK37 1
#define APP_NAME "SMKSTAGE"
#define OBV_DUAL_CORE 1
/* K8 controls drawbar 8, not the platform emulated MASTER pot. */
#define SMK_CONTROLLER_ACTIVE 1
/* Stage-exclusive sectors beyond the SMKSYNTH pattern slots. Never erase foreign data. */
#define STAGE_STORE_A 0x000E8000u
#define STAGE_STORE_B 0x000E9000u
#define STAGE_EXTRA_RANGE(off,n) (FL_IN(off,n,STAGE_STORE_A,STAGE_STORE_B+0x1000u))
/* Give keyboard events priority while the LCD DMA is in flight. */
static void keys_tick(void);
#define LCD_IDLE() keys_tick()
#include "platform.c"
#include "organ_tab.h"
/* Keep Leslie/reverb delay lines in the large .pool, not 96 KiB system RAM. */
#define ORGAN_POOL __attribute__((section(".pool")))
#define STAGE_PLATE 1
static uint8_t stage_plate_size=77u,stage_plate_tone=69u,stage_plate_pre=10u;
#include "smkstage_plate.h"
#define ORGAN_C1_RUN(fn,arg) fm1_mc_run((fn),(arg))
#define ORGAN_C1_WAIT() fm1_mc_wait()
static volatile uint8_t stage_les_bypass;
#include "organ_core.h"
static uint8_t stage_manual, stage_bank, stage_page, stage_f1_caught;
static uint8_t stage_store_dirty,stage_save_now,stage_store_status,stage_data_locked;
static uint32_t stage_store_change_ms,stage_store_last_ms,stage_store_seq,stage_store_sector=STAGE_STORE_B;
static uint8_t stage_restore_done;
static uint8_t held_manual[37], held_active[37], pad_down[16];
static uint32_t pad_start[16], draw_ms, leds_ms;
static uint8_t favorites[2][8][3][9], saved[2][8], chosen;
static uint8_t full[8][12];
static uint16_t favorite_les[8][8],favorite_ton[8][2],favorite_eq[8][6];
static uint8_t favorite_plate[8][3],favorite_bypass[8],favorite_chr[8];
static uint8_t *db(void){return stage_manual==0?(uint8_t*)P.db:stage_manual==1?(uint8_t*)P.dbl:(uint8_t*)P.dbp;}
static uint32_t clamp(int32_t v,uint32_t max){return v<0?0:(uint32_t)v>max?max:(uint32_t)v;}
static void stage_slot_save(uint32_t p){
 if(p>7)return;
 for(uint32_t m=0;m<3;m++)for(uint32_t i=0;i<9;i++)
  favorites[stage_bank][p][m][i]=m==0?P.db[i]:m==1?P.dbl[i]:(i<2?P.dbp[i]:0);
 saved[stage_bank][p]=1;chosen=(uint8_t)p;
 if(stage_bank){full[p][0]=P.perc;full[p][1]=P.perc3;full[p][2]=P.soft;full[p][3]=P.fast;
 full[p][4]=P.vup;full[p][5]=P.vlo;full[p][6]=P.vtype;full[p][7]=P.od;full[p][8]=P.drv;
 full[p][9]=P.rev;full[p][10]=P.swell;full[p][11]=stage_manual;
 for(uint32_t i=0;i<8;i++)favorite_les[p][i]=les_p[i];
 for(uint32_t i=0;i<2;i++)favorite_ton[p][i]=ton_p[i];
 for(uint32_t i=0;i<6;i++)favorite_eq[p][i]=eq_p[i];
 favorite_plate[p][0]=stage_plate_size;favorite_plate[p][1]=stage_plate_tone;favorite_plate[p][2]=stage_plate_pre;
 favorite_bypass[p]=stage_les_bypass;favorite_chr[p]=P.chr;}
 stage_store_dirty=1;stage_store_change_ms=fm1_ms;
}
static void stage_slot_load(uint32_t p){
 if(p>7||!saved[stage_bank][p])return;chosen=(uint8_t)p;
 if(stage_bank){for(uint32_t m=0;m<3;m++)for(uint32_t i=0;i<9;i++){
 uint8_t v=favorites[1][p][m][i];if(m==0)P.db[i]=v;else if(m==1)P.dbl[i]=v;else if(i<2)P.dbp[i]=v;}
 P.perc=full[p][0];P.perc3=full[p][1];P.soft=full[p][2];P.fast=full[p][3];P.vup=full[p][4];P.vlo=full[p][5];
 P.vtype=full[p][6];P.od=full[p][7];P.drv=full[p][8];P.rev=full[p][9];P.swell=full[p][10];stage_manual=full[p][11];
 P.chr=favorite_chr[p];stage_les_bypass=favorite_bypass[p];
 for(uint32_t i=0;i<8;i++)les_set(i,favorite_les[p][i]);
 for(uint32_t i=0;i<2;i++)ton_set(i,favorite_ton[p][i]);
 eq_set(favorite_eq[p]);
 stage_plate_size=favorite_plate[p][0];stage_plate_tone=favorite_plate[p][1];stage_plate_pre=favorite_plate[p][2];
 ovp_plate_params(stage_plate_size,stage_plate_tone,stage_plate_pre);
 }else{for(uint32_t m=0;m<3;m++)for(uint32_t i=0;i<(m==2?2u:9u);i++){
 uint8_t v=favorites[0][p][m][i];if(m==0)P.db[i]=v;else if(m==1)P.dbl[i]=v;else P.dbp[i]=v;
 }}
 stage_f1_caught=0;stage_store_dirty=1;stage_store_change_ms=fm1_ms;
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
 if(b&(1u<<BTN_EDIT))stage_page=(stage_page+1u)%9u;
 if(b&(1u<<BTN_HOME))stage_page=0;
 if(b&(1u<<BTN_SAVE)){stage_save_now=1;stage_store_dirty=1;stage_store_change_ms=fm1_ms;}
 for(uint32_t i=0;i<8;i++){int32_t v=fm1_enc_take(i);if(!v)continue;
 if(stage_page==0){uint8_t *d=db();if(stage_manual!=2||i<2)d[i]=(uint8_t)clamp((int32_t)d[i]+v,8);}
 else if(stage_page==1&&i<4u)les_set(i,clamp((int32_t)les_p[i]+v*5,(uint32_t)LES_MAX[i]));
 else if(stage_page==2&&i<4u)les_set(i+4u,clamp((int32_t)les_p[i+4u]+v*25,(uint32_t)LES_MAX[i+4u]));
 else if(stage_page==3){if(i==0)ton_set(0,clamp((int32_t)ton_p[0]+v*10u,4500));else if(i==1)ton_set(1,clamp((int32_t)ton_p[1]+v*10u,1000));}
 else if(stage_page==4){if(i==0)P.drv=(uint8_t)clamp((int32_t)P.drv+v,20);else if(i==1)P.chr=(uint8_t)clamp((int32_t)P.chr+v,127);}
 else if(stage_page==5){if(i==0)stage_plate_size=(uint8_t)clamp((int32_t)stage_plate_size+v,100);else if(i==1)stage_plate_tone=(uint8_t)clamp((int32_t)stage_plate_tone+v,100);else if(i==2)stage_plate_pre=(uint8_t)clamp((int32_t)stage_plate_pre+v,100);else if(i==3)P.rev=(uint8_t)clamp((int32_t)P.rev+v,127);ovp_plate_params(stage_plate_size,stage_plate_tone,stage_plate_pre);}
 else if(stage_page==6&&i<6u){uint16_t p[6];for(uint32_t j=0;j<6;j++)p[j]=eq_p[j];p[i]=(uint16_t)clamp((int32_t)p[i]+v*(i&1u? (i==5u?100:20):1),EQ_MAX[i]);if(p[i]<EQ_MIN[i])p[i]=EQ_MIN[i];eq_set(p);}
 else if(stage_page==7){if(i==0)P.vtype=(uint8_t)clamp((int32_t)P.vtype+v,5);else if(i==1)P.vup^=1u;else if(i==2)P.vlo^=1u;else if(i==3)P.perc^=1u;}
 if(stage_page!=8u){stage_store_dirty=1;stage_store_change_ms=fm1_ms;}
 }
 static uint8_t last[4];
 for(uint32_t i=0;i<4;i++){uint8_t f=fm1_in.fader[i],prev=last[i];if(last[i]==f)continue;last[i]=f;
 if(i==0){uint8_t *d=db(),at=stage_manual==2?1:8,val=(uint8_t)(8u-(f*8u+63u)/127u);
  if(!stage_f1_caught){uint8_t was=(uint8_t)(8u-(prev*8u+63u)/127u);if(val==d[at]||(was<=d[at]&&val>=d[at])||(was>=d[at]&&val<=d[at]))stage_f1_caught=1;else continue;}d[at]=val;
 }else if(i==1)P.swell=f;else if(i==2)P.drv=(uint8_t)((f*20u+63u)/127u);else P.rev=f;
 stage_store_dirty=1;stage_store_change_ms=fm1_ms;}
 int32_t event;while((event=smk_pad_take())>=0){
 uint32_t p=((uint32_t)event>>8)&15u,on=(uint32_t)event&255u;
 if(on){pad_down[p]=1;pad_start[p]=fm1_ms;if(p>=8&&p!=9){pad_press(p);stage_store_dirty=1;stage_store_change_ms=fm1_ms;}}
 else if(pad_down[p]){uint32_t duration=fm1_ms-pad_start[p];pad_down[p]=0;
  if(p==9){if(duration>=500u)P.les=0;else P.les=P.les==0?1:P.les==1?2:1;stage_store_dirty=1;stage_store_change_ms=fm1_ms;}
  else if(p<8){if(duration>=600u)stage_slot_save(p);else stage_slot_load(p);}
 }}
}
static void keys_tick(void){
 uint64_t k;uint32_t irq=fm1__lock();k=fm1_in.keys;fm1__unlock(irq);
 for(uint32_t i=0;i<37;i++){uint8_t on=(uint8_t)((k>>i)&1u);if(on==held_active[i])continue;
 if(on){held_manual[i]=stage_manual;organ_note(stage_manual,1,48+i,1,1);}
 else organ_note(held_manual[i],1,48+i,0,1);
 held_active[i]=on;}
}
static void leds(void){
 for(uint32_t i=0;i<16;i++){uint32_t r=0,g=0,b=0;
 if(i<8){r=saved[stage_bank][i]?90:8;g=chosen==i?50:0;}
 else switch(i){
 case 8:if(!stage_les_bypass)r=g=b=255;break;
 case 9:if(!stage_les_bypass){/* Real rotor phase: same principle as the SMKTRON indicator, never a slow fake timer. */
 uint32_t phase=ls_hph>>24,v=phase<128u?phase:255u-phase;r=g=b=P.les==0?40u:25u+v*230u/127u;}break;
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
/* Rendering on SMK-37 is expensive: update only when parameters change.
 * No full-screen clearing in the steady state, and note-on/off is serviced
 * while lcd_sync waits for DMA (LCD_IDLE).
 */
static uint32_t stage_sig(void){
 uint32_t h=2166136261u;
 #define STAGE_MIX(x) do{h=((h ^ (uint32_t)(x))*16777619u);}while(0)
 STAGE_MIX(stage_manual);STAGE_MIX(stage_bank);STAGE_MIX(stage_page);STAGE_MIX(chosen);
 for(uint32_t m=0;m<3;m++){const volatile uint8_t *p=m==0?P.db:m==1?P.dbl:P.dbp;for(uint32_t i=0;i<(m==2?2u:9u);i++)STAGE_MIX(p[i]);}
 STAGE_MIX(P.perc);STAGE_MIX(P.perc3);STAGE_MIX(P.fast);STAGE_MIX(P.soft);
 STAGE_MIX(P.vtype);STAGE_MIX(P.vup);STAGE_MIX(P.vlo);STAGE_MIX(P.les);
 STAGE_MIX(P.od);STAGE_MIX(P.drv);STAGE_MIX(P.chr);STAGE_MIX(P.rev);STAGE_MIX(P.swell);STAGE_MIX(stage_les_bypass);
 if(stage_page==1u||stage_page==2u)for(uint32_t i=0;i<8;i++)STAGE_MIX(les_p[i]);
 STAGE_MIX(ton_p[0]);STAGE_MIX(ton_p[1]);STAGE_MIX(stage_plate_size);STAGE_MIX(stage_plate_tone);STAGE_MIX(stage_plate_pre);STAGE_MIX(stage_store_status);
 if(stage_page==6u)for(uint32_t i=0;i<6;i++)STAGE_MIX(eq_p[i]);
 #undef STAGE_MIX
 return h;
}
static void stage_number(char *s,uint32_t val){
 char b[12];uint32_t n=0,i=0;do{b[n++]=(char)('0'+val%10u);val/=10u;}while(val&&n<11);
 while(n)s[i++]=b[--n];s[i]=0;
}
static void stage_kv(int32_t x,int32_t y,const char *tag,uint32_t value){
 char line[36];uint32_t i=0,k=0;for(;tag[i]&&i<24;i++)line[i]=tag[i];
 line[i++]=' ';stage_number(line+i,value);
 cv_text(x,y,&FONT_S,line,C_WHITE);
}
static void screen_draw(void){
 uint8_t *d=db();uint32_t count=stage_manual==2?2:9;
 draw_text_box(0,2,240,&FONT_S,"SMKSTAGE B3",C_WHITE,1);
 draw_text_box(0,25,240,&FONT_S,stage_manual==0?"UPPER":stage_manual==1?"LOWER":"PEDAL",RGB(240,55,65),1);
 draw_text_box(0,47,240,&FONT_S,stage_bank?"BANK B FAVORITES":"BANK A DRAWBARS",C_WHITE,1);
 cv_begin(240,128,C_BLACK);
 if(stage_page==0){
  for(uint32_t j=0;j<9;j++){
   int32_t x=4+(int32_t)j*26;uint32_t v=j<count?d[j]:0;
   cv_rect(x,6,19,103,RGB(35,38,43));
   /* Hammond: 0 = pushed all the way up, 8 = pulled down. */
   cv_rect(x+7,8,5,10+(int32_t)v*10,RGB(140,143,151));
   cv_rect(x,10+(int32_t)v*10,19,13,j<count?RGB(220,53,64):RGB(70,70,75));
   char n[2]={(char)('0'+v),0};
   cv_text(x+5,111,&FONT_S,n,j<count?C_WHITE:RGB(70,70,70));
  }
 }else if(stage_page==1){
  for(uint32_t i=0;i<8;i++){
   char tag[4]={'K',(char)('1'+i),':',0};
   int32_t x=(int32_t)(i&1u)*120,y=7+(int32_t)(i>>1u)*27;
   stage_kv(x+5,y,tag,les_p[i]);
  }
 }else if(stage_page==2){
  stage_kv(8,12,"K1 DRIVE",P.drv);
  stage_kv(8,36,"K2 CHAR",P.chr);
  stage_kv(8,60,"F3 DRIVE",P.drv);
  stage_kv(8,84,"F4 REVERB",P.rev);
 }else if(stage_page==3){
  stage_kv(8,12,"K1 VIB TYPE",P.vtype+1u);
  stage_kv(8,36,"UPPER VIB",P.vup);
  stage_kv(8,60,"LOWER VIB",P.vlo);
  stage_kv(8,84,"PERC",P.perc);
 }
 cv_blit(0,68);
 /* The footer is page-specific, not just a changing label on drawbars. */
 draw_text_box(0,205,240,&FONT_S,stage_page==0?"K1-8 DRAWBARS F1=9":stage_page==1?"LESLIE: K1-8":stage_page==2?"DRIVE / CHARACTER":"VIBRATO / PERC",C_WHITE,1);
 draw_text_box(0,223,240,&FONT_S,"PARA:NEXT  PATCH:BACK",RGB(145,149,162),1);
}
static void app_main(void){
 uint8_t st,a,b;uint32_t last_sig=0;uint8_t first_draw=1;smk_pad_events=1;smk_pad_grid=1;palette_set(3);lcd_fill(0,0,240,240,C_BLACK);
 organ_program(5u);P.les=1;
 for(;;){platform_poll();keys_tick();controls();
 while(midi_get(&st,&a,&b)){uint32_t ch=st&15u;
 switch(st&0xF0u){case 0x90u:organ_midi_note(ch,a,b!=0);break;case 0x80u:organ_midi_note(ch,a,0);break;
 case 0xB0u:organ_cc_ch(ch,a,b);break;case 0xC0u:organ_pc(a);break;}}
 if(fm1_ms-leds_ms>=40u){leds_ms=fm1_ms;leds();}
 if(first_draw || (fm1_ms-draw_ms>=75u && stage_sig()!=last_sig)){draw_ms=fm1_ms;last_sig=stage_sig();first_draw=0;screen_draw();}
 }}
