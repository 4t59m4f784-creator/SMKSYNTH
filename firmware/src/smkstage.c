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
/* D4 EXPERIMENT ONLY: all 91 tonewheels rendered on CPU0 in the SAME
 * original wheels_render(0,1,...) path. CPU1 stays booted/idle, no wheel
 * jobs dispatched. No change to Leslie, chorus, plate, samples, controls,
 * keybed scan, interrupt priorities or flash journal.
 * This is for isolating CPU1 handoff from the repeatable PC-limit crash.
 * Expected higher CPU0 load; never an optimized production option. */
#define STAGE_SINGLE_CORE_TONEWHEELS 1
#define ORGAN_C1_RUN(fn,arg) 0
#define ORGAN_C1_WAIT() 0
static volatile uint8_t stage_les_bypass;
/* Audio ISR starts before app_main. Do not render until plate delay-line pointers,
 * factory program state and store journal are initialized. */
static volatile uint8_t stage_audio_ready;
#include "organ_core.h"
static uint8_t stage_manual, stage_bank, stage_page, stage_f1_caught;
static uint8_t stage_library_program=5u; /* original setBfree registration IDs 1..127 */
static uint8_t stage_store_dirty,stage_save_now,stage_store_status,stage_data_locked;
static uint32_t stage_store_change_ms,stage_store_last_ms,stage_store_seq,stage_store_sector=STAGE_STORE_B;
/* The pending indicator stays visible until flash verification succeeds. */
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

/* Stage's own journal: two sectors E8000/E9000; SMKSYNTH patterns stop at E2000.
 * Do not write an occupied sector unless it contains a valid Stage record.
 * The previous valid sector survives a power failure during a new save.
 * Unlike the SMKSYNTH owner routine, this never clears foreign storage. */
_Static_assert(STAGE_STORE_A>=SLOTS_AT+NBLOB*SLOT_SECT*4096u,"B3 journal overlaps SMKSYNTH patterns");
_Static_assert(STAGE_STORE_B+4096u<=FL_SMP_HI,"B3 journal outside allowed sample window");
#define STAGE_TAG 0x33424753u
#define STAGE_REV 3u
typedef struct {
 uint32_t tag,version,sequence;
 uint8_t db[9],dbl[9],dbp[2];
 uint8_t sound[16],manual,bank,chosen,bypass;
 uint8_t fav[2][8][3][9],valid[2][8],full[8][12];
 uint16_t lf[8][8],tf[8][2],ef[8][6];
 uint8_t pf[8][3],bf[8],cf[8],plate[3];
 uint16_t les[8],ton[2],eq[6];
} stage_record_t;
static stage_record_t stage_last_record;
static uint8_t stage_have_record;
static void stage_capture(stage_record_t *r){
 memset(r,0,sizeof *r);r->tag=STAGE_TAG;r->version=STAGE_REV;
 for(uint32_t j=0;j<9;j++){r->db[j]=P.db[j];r->dbl[j]=P.dbl[j];}
 for(uint32_t j=0;j<2;j++)r->dbp[j]=P.dbp[j];
 const uint8_t q[16]={P.vtype,P.vup,P.vlo,P.perc,P.perc3,P.fast,P.soft,P.les,P.rev,P.od,P.drv,P.swell,P.pgm,P.chr,0,0};
 memcpy(r->sound,q,sizeof q);r->manual=stage_manual;r->bank=stage_bank;r->chosen=chosen;r->bypass=stage_les_bypass;
 memcpy(r->fav,favorites,sizeof favorites);memcpy(r->valid,saved,sizeof saved);memcpy(r->full,full,sizeof full);
 memcpy(r->lf,favorite_les,sizeof favorite_les);memcpy(r->tf,favorite_ton,sizeof favorite_ton);memcpy(r->ef,favorite_eq,sizeof favorite_eq);
 memcpy(r->pf,favorite_plate,sizeof favorite_plate);memcpy(r->bf,favorite_bypass,sizeof favorite_bypass);memcpy(r->cf,favorite_chr,sizeof favorite_chr);
 r->plate[0]=stage_plate_size;r->plate[1]=stage_plate_tone;r->plate[2]=stage_plate_pre;
 for(uint32_t j=0;j<8;j++)r->les[j]=les_p[j];
 for(uint32_t j=0;j<2;j++)r->ton[j]=ton_p[j];
 for(uint32_t j=0;j<6;j++)r->eq[j]=eq_p[j];
}
static void stage_restore(const stage_record_t *r){
 for(uint32_t j=0;j<9;j++){P.db[j]=r->db[j];P.dbl[j]=r->dbl[j];}
 for(uint32_t j=0;j<2;j++)P.dbp[j]=r->dbp[j];
 P.vtype=r->sound[0];P.vup=r->sound[1];P.vlo=r->sound[2];P.perc=r->sound[3];P.perc3=r->sound[4];
 P.fast=r->sound[5];P.soft=r->sound[6];P.les=r->sound[7];P.rev=r->sound[8];P.od=r->sound[9];
 P.drv=r->sound[10];P.swell=r->sound[11];P.pgm=r->sound[12];P.chr=r->sound[13];
 stage_manual=r->manual;stage_bank=r->bank;chosen=r->chosen;stage_les_bypass=r->bypass;
 memcpy(favorites,r->fav,sizeof favorites);memcpy(saved,r->valid,sizeof saved);memcpy(full,r->full,sizeof full);
 memcpy(favorite_les,r->lf,sizeof favorite_les);memcpy(favorite_ton,r->tf,sizeof favorite_ton);memcpy(favorite_eq,r->ef,sizeof favorite_eq);
 memcpy(favorite_plate,r->pf,sizeof favorite_plate);memcpy(favorite_bypass,r->bf,sizeof favorite_bypass);memcpy(favorite_chr,r->cf,sizeof favorite_chr);
 stage_plate_size=r->plate[0];stage_plate_tone=r->plate[1];stage_plate_pre=r->plate[2];
 for(uint32_t j=0;j<8;j++)les_set(j,r->les[j]);
 for(uint32_t j=0;j<2;j++)ton_set(j,r->ton[j]);
 eq_set(r->eq);ovp_plate_params(stage_plate_size,stage_plate_tone,stage_plate_pre);
 stage_f1_caught=0;
}
static int stage_valid(const stage_record_t *r){
 if(r->tag!=STAGE_TAG||r->version!=STAGE_REV||r->manual>2||r->bank>1||r->chosen>7||
 r->sound[0]>5||r->sound[7]>2||r->sound[8]>127||r->sound[10]>20||r->sound[13]>127||
 r->plate[0]>100||r->plate[1]>100||r->plate[2]>100)return 0;
 for(uint32_t i=0;i<9;i++)if(r->db[i]>8||r->dbl[i]>8)return 0;
 for(uint32_t i=0;i<2;i++)if(r->dbp[i]>8)return 0;
 for(uint32_t i=0;i<8;i++)if(r->les[i]<LES_MIN[i]||r->les[i]>LES_MAX[i])return 0;
 for(uint32_t i=0;i<2;i++)if(r->ton[i]<(i?0u:4300u)||r->ton[i]>(i?1000u:4500u))return 0;
 for(uint32_t i=0;i<6;i++)if(r->eq[i]<EQ_MIN[i]||r->eq[i]>EQ_MAX[i])return 0;
 return 1;
}
static uint8_t stage_sector_state(uint32_t off,stage_record_t *r){
 uint32_t h[3];if(!flash_ok||st_read(off,h,sizeof h))return 0;
 if(h[0]==0xFFFFFFFFu&&h[1]==0xFFFFFFFFu&&h[2]==0xFFFFFFFFu){
  /* A blank header alone is insufficient: an interrupted write leaves payload. */
  uint32_t check[16];
  uint32_t off_sector=off;
  for(uint32_t ofs=0;ofs<4096u;ofs+=sizeof check){
   if(st_read(off_sector + ofs,check,sizeof check))return 0;
   for(uint32_t i=0;i<16u;i++)if(check[i]!=0xFFFFFFFFu)return 0;
  }
  return 1;
 } /* genuinely erased */
 if(!sector_load(off,r,sizeof *r)&&stage_valid(r))return 2; /* our valid record */
 return 0; /* foreign or interrupted record: never erase unknown contents */
}
static void stage_store_boot(void){
 static stage_record_t a,b;
 uint8_t sa=stage_sector_state(STAGE_STORE_A,&a),sb=stage_sector_state(STAGE_STORE_B,&b);
 if(!sa||!sb){/* Restore a surviving verified copy, but never erase an unknown/interrupted sector. */
 if(sa==2u){stage_last_record=a;stage_restore(&a);stage_have_record=1;}
 else if(sb==2u){stage_last_record=b;stage_restore(&b);stage_have_record=1;}
 stage_data_locked=1;stage_store_status=3;return;}
 if(sa==2u&&sb==2u){stage_last_record=(int32_t)(a.sequence-b.sequence)>=0?a:b;stage_store_sector=stage_last_record.sequence==a.sequence?STAGE_STORE_A:STAGE_STORE_B;}
 else if(sa==2u){stage_last_record=a;stage_store_sector=STAGE_STORE_A;}
 else if(sb==2u){stage_last_record=b;stage_store_sector=STAGE_STORE_B;}
 else {stage_store_status=0;return;}
 stage_store_seq=stage_last_record.sequence;stage_have_record=1;stage_restore(&stage_last_record);stage_store_status=1;
}
static void stage_store_commit(void){
 static stage_record_t r,check;
 if(stage_data_locked||!flash_ok){stage_store_status=3;return;}
 stage_capture(&r);
 if(stage_have_record){r.sequence=stage_last_record.sequence;if(!memcmp(&r,&stage_last_record,sizeof r)){stage_store_dirty=0;stage_save_now=0;stage_store_status=1;return;}}
 r.sequence=stage_store_seq+1u;
 uint32_t next=stage_store_sector==STAGE_STORE_A?STAGE_STORE_B:STAGE_STORE_A;
 if(!stage_sector_state(next,&check)){stage_data_locked=1;stage_store_status=3;return;}
 if(sector_save(next,&r,sizeof r)==0&&!sector_load(next,&check,sizeof check)&&stage_valid(&check)&&!memcmp(&r,&check,sizeof r)){
  stage_store_sector=next;stage_store_seq=r.sequence;stage_last_record=r;stage_have_record=1;
  stage_store_dirty=0;stage_save_now=0;stage_store_status=1;stage_store_last_ms=fm1_ms;
 }else{stage_data_locked=1;stage_store_status=3;}
}
static void stage_factory_reset_sound(void){
 P.db[0]=8;P.db[1]=8;P.db[2]=8;
 for(uint32_t j=3;j<9;j++)P.db[j]=0;
 const uint8_t lower[9]={8,3,8,0,0,0,0,0,0};
 for(uint32_t j=0;j<9;j++)P.dbl[j]=lower[j];
 P.dbp[0]=8;P.dbp[1]=6;
 P.perc=0;P.perc3=0;P.fast=1;P.soft=0;P.vtype=0;P.vup=0;P.vlo=0;
 P.les=1;P.od=0;P.drv=12;P.chr=16;P.rev=32;P.swell=127;
 stage_manual=0;stage_les_bypass=0;stage_plate_size=77u;stage_plate_tone=69u;stage_plate_pre=10u;
 for(uint32_t j=0;j<8;j++)les_set(j,LES_DEF[j]);
 for(uint32_t j=0;j<2;j++)ton_set(j,TON_DEF[j]);
 eq_set(EQ_DEF);ovp_plate_params(stage_plate_size,stage_plate_tone,stage_plate_pre);
}
static void stage_factory_slots(void){
 static const uint8_t source[8]={5,104,7,15,18,107,106,108};
 for(uint32_t i=0;i<8;i++){
  stage_factory_reset_sound();organ_program(source[i]);stage_bank=0;stage_slot_save(i);
  stage_bank=1;stage_slot_save(i);
 }
 stage_factory_reset_sound();organ_program(5u);stage_bank=0;chosen=0;
 stage_store_dirty=0;
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
/* Physical SMK-37 mod wheel, as calibrated in SMKSYNTH:
 * up = smaller ADC number, down = larger ADC number.
 * Leslie only uses SLOW/FAST; hysteresis avoids chattering at mid travel.
 * No STOP here: pad 10 remains responsible for STOP.
 */
static void stage_mod_leslie(void){
 static uint8_t initialized;
 static uint8_t mode;
 uint16_t raw;
 if(fm1_in.frames<300u)return; /* settle the ADC, same as SMKSYNTH */
 raw=fm1_in.wheel[1];
 if(!initialized){
  mode=raw<550u?2u:1u;
  initialized=1;
  P.les=mode;
  return;
 }
 uint8_t next=mode;
 if(mode==1u&&raw<520u)next=2u;
 else if(mode==2u&&raw>580u)next=1u;
 /* Only a physical wheel state transition sends a Leslie command.
  * Pad 10, saved presets and MIDI can select STOP/FAST/SLOW independently
  * without being overwritten continuously by a stationary wheel. */
 if(next!=mode){
  mode=next;
  P.les=mode;
  stage_store_dirty=1;
  stage_store_change_ms=fm1_ms;
 }
}
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
 else if(stage_page==8u&&i==0u){
  /* K1 previews the selected setBfree registration; pad bank B captures a favorite. */
  uint32_t target=stage_library_program;int32_t step=v>0?1:-1,remaining=v>0?v:-v;
  if(remaining>16)remaining=16;
  while(remaining-->0){uint32_t next=pgm_step(target,step);if(next==target)break;target=next;}
  if(target!=stage_library_program){stage_library_program=(uint8_t)target;organ_program(target);}
 }
 else if(stage_page==1&&i<4u)les_set(i,clamp((int32_t)les_p[i]+v*5,(uint32_t)LES_MAX[i]));
 else if(stage_page==2&&i<4u)les_set(i+4u,clamp((int32_t)les_p[i+4u]+v*25,(uint32_t)LES_MAX[i+4u]));
 else if(stage_page==3){if(i==0)ton_set(0,clamp((int32_t)ton_p[0]+v*10u,4500));else if(i==1)ton_set(1,clamp((int32_t)ton_p[1]+v*10u,1000));}
 else if(stage_page==4){if(i==0)P.drv=(uint8_t)clamp((int32_t)P.drv+v,20);else if(i==1)P.chr=(uint8_t)clamp((int32_t)P.chr+v,127);}
 else if(stage_page==5){if(i==0)stage_plate_size=(uint8_t)clamp((int32_t)stage_plate_size+v,100);else if(i==1)stage_plate_tone=(uint8_t)clamp((int32_t)stage_plate_tone+v,100);else if(i==2)stage_plate_pre=(uint8_t)clamp((int32_t)stage_plate_pre+v,100);else if(i==3)P.rev=(uint8_t)clamp((int32_t)P.rev+v,127);ovp_plate_params(stage_plate_size,stage_plate_tone,stage_plate_pre);}
 else if(stage_page==6&&i<6u){uint16_t p[6];for(uint32_t j=0;j<6;j++)p[j]=eq_p[j];p[i]=(uint16_t)clamp((int32_t)p[i]+v*(i&1u? (i==5u?100:20):1),EQ_MAX[i]);if(p[i]<EQ_MIN[i])p[i]=EQ_MIN[i];eq_set(p);}
 else if(stage_page==7){if(i==0)P.vtype=(uint8_t)clamp((int32_t)P.vtype+v,5);else if(i==1)P.vup^=1u;else if(i==2)P.vlo^=1u;else if(i==3)P.perc^=1u;}
 {stage_store_dirty=1;stage_store_change_ms=fm1_ms;}
 }
 static uint8_t last[4];
 for(uint32_t i=0;i<4;i++){uint8_t f=fm1_in.fader[i];if(last[i]==f)continue;last[i]=f;
 if(i==0){uint8_t *d=db(),at=stage_manual==2?1:8,val=(uint8_t)(8u-(f*8u+63u)/127u);
  /* The physical drawbar must respond immediately: bottom=0, top=8.
   * Do not require crossing an invisible pickup position on a loaded preset. */
  d[at]=val;
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
 STAGE_MIX(stage_manual);STAGE_MIX(stage_bank);STAGE_MIX(stage_page);STAGE_MIX(chosen);STAGE_MIX(stage_library_program);
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
 uint8_t *d=db();uint32_t count=stage_manual==2?2u:9u;
 static const char *const pages[9]={"PERFORMANCE","LESLIE SPEED","LESLIE RAMP","TONEWHEELS","DRIVE","PLATE REVERB","3 BAND EQ","VIB / PERC","PRESET LIBRARY"};
 lcd_sync();
 draw_text_box(0,2,240,&FONT_S,"SMKSTAGE B3",C_WHITE,1);
 draw_text_box(0,23,240,&FONT_S,stage_manual==0?"UPPER":stage_manual==1?"LOWER":"PEDAL",RGB(245,65,77),1);
 draw_text_box(0,44,240,&FONT_S,pages[stage_page%9u],C_WHITE,1);
 cv_begin(240,128,C_BLACK);
 if(stage_page==0u){
  for(uint32_t j=0;j<9;j++){
   int32_t x=4+(int32_t)j*26;uint32_t v=j<count?d[j]:0;
   cv_rect(x,6,19,103,RGB(35,38,43));
   cv_rect(x+7,8,5,10+(int32_t)v*10,RGB(140,143,151));
   cv_rect(x,10+(int32_t)v*10,19,13,j<count?RGB(220,53,64):RGB(70,70,75));
   char n[2]={(char)('0'+v),0};
   cv_text(x+5,111,&FONT_S,n,j<count?C_WHITE:RGB(70,70,70));
  }
 }else if(stage_page==1u){
  stage_kv(7,8,"K1 HORN SLOW",les_p[0]);
  stage_kv(7,35,"K2 HORN FAST",les_p[1]);
  stage_kv(7,62,"K3 DRUM SLOW",les_p[2]);
  stage_kv(7,89,"K4 DRUM FAST",les_p[3]);
 }else if(stage_page==2u){
  stage_kv(7,8,"K1 HORN ACC",les_p[4]);
  stage_kv(7,35,"K2 HORN DEC",les_p[5]);
  stage_kv(7,62,"K3 DRUM ACC",les_p[6]);
  stage_kv(7,89,"K4 DRUM DEC",les_p[7]);
 }else if(stage_page==3u){
  stage_kv(7,8,"K1 TUNE x0.1Hz",ton_p[0]);
  stage_kv(7,35,"K2 KEY CLICK",ton_p[1]);
  cv_text(7,65,&FONT_S,"91 ORIGINAL WHEELS",RGB(170,175,185));
  cv_text(7,88,&FONT_S,"LEAKAGE: SETBFREE",RGB(170,175,185));
 }else if(stage_page==4u){
  stage_kv(7,12,"K1 DRIVE",P.drv);
  stage_kv(7,41,"K2 CHARACTER",P.chr);
  stage_kv(7,70,"DRIVE ON",P.od);
 }else if(stage_page==5u){
  stage_kv(7,8,"K1 SIZE",stage_plate_size);
  stage_kv(7,35,"K2 TONE",stage_plate_tone);
  stage_kv(7,62,"K3 PREDELAY",stage_plate_pre);
  stage_kv(7,89,"K4 MIX",P.rev);
 }else if(stage_page==6u){
  stage_kv(7,8,"K1 LOW dB INDEX",eq_p[0]);
  stage_kv(7,28,"K2 LOW Hz",eq_p[1]);
  stage_kv(7,48,"K3 MID dB INDEX",eq_p[2]);
  stage_kv(7,68,"K4 MID Hz",eq_p[3]);
  stage_kv(7,88,"K5 HIGH dB INDEX",eq_p[4]);
  stage_kv(7,108,"K6 HIGH Hz",eq_p[5]);
 }else if(stage_page==8u){
  stage_kv(7,7,"SETBFREE PRESET",stage_library_program);
  const char *name=pgm_name(stage_library_program);
  char buf[23];uint32_t k=0;
  if(name)for(;k<21u&&name[k];k++)buf[k]=name[k];
  buf[k]=0;
  cv_text(7,43,&FONT_S,buf,C_WHITE);
  cv_text(7,76,&FONT_S,"K1: BROWSE + LOAD",RGB(180,184,196));
  cv_text(7,100,&FONT_S,"BANK B: HOLD=FAV",RGB(180,184,196));
 }else if(stage_page==7u){
  stage_kv(7,8,"K1 VIB TYPE",P.vtype+1u);
  stage_kv(7,35,"K2 UPPER VIB",P.vup);
  stage_kv(7,62,"K3 LOWER VIB",P.vlo);
  stage_kv(7,89,"K4 PERC ON",P.perc);
 }
 cv_blit(0,68);
 draw_text_box(0,204,240,&FONT_S,stage_page==0?"K1-8 BARS / F1=9":stage_page==1?"0.1 RPM / HORN+DRUM":stage_page==2?"ACC/DEC = MILLISECONDS":stage_page==3?"LEAKAGE MODEL FIXED":stage_page==4?"PAD16=OVERDRIVE":stage_page==5?"DATTORRO / F4 MIX":stage_page==6?"GAIN 18=FLAT  (+/-9DB)":stage_page==7u?"PERC: PAD11":"57 ORIGINAL REGISTRATIONS",RGB(190,198,211),1);
 draw_text_box(0,222,240,&FONT_S,stage_store_status==3u?"FLASH LOCKED - NO SAVE":stage_store_status==2u?"UNSAVED  BT:SAVE":stage_store_status==1u?"SAVED  BT:SAVE":"BT:SAVE / PARA:NEXT",stage_store_status==3u?RGB(255,110,90):C_WHITE,1);
}
/* Drawbar-only updates use one 26x124 canvas lane (~6.4 KiB DMA)
 * instead of the complete 240x124 canvas (~60 KiB). This reduces
 * SPI/bus contention precisely during rapid encoder movement.
 * Full page changes still render the complete page once. */
static uint8_t stage_screen_db[9],stage_screen_page=255u,stage_screen_manual=255u,stage_screen_bank=255u;
static void stage_screen_snapshot(void){
 const uint8_t *d=db();
 for(uint32_t j=0;j<9u;j++)stage_screen_db[j]=(uint8_t)(j<(stage_manual==2u?2u:9u)?d[j]:0u);
 stage_screen_page=stage_page;stage_screen_manual=stage_manual;stage_screen_bank=stage_bank;
}
static void stage_draw_lane(uint32_t j,uint32_t v,uint32_t enabled){
 cv_begin(26u,124u,C_BLACK);
 cv_rect(0,6,19,103,RGB(35,38,43));
 cv_rect(7,8,5,10+(int32_t)v*10,RGB(140,143,151));
 cv_rect(0,10+(int32_t)v*10,19,13,enabled?RGB(220,53,64):RGB(70,70,75));
 char n[2]={(char)('0'+v),0};
 cv_text(5,111,&FONT_S,n,enabled?C_WHITE:RGB(70,70,70));
 cv_blit(4u+j*26u,68u);
}
static void stage_screen_update(void){
 if(stage_page!=0u||stage_screen_page!=0u||stage_screen_manual!=stage_manual||stage_screen_bank!=stage_bank){
  screen_draw();stage_screen_snapshot();return;
 }
 const uint8_t *d=db();uint32_t count=stage_manual==2u?2u:9u;
 for(uint32_t j=0;j<9u;j++){
  uint8_t v=(uint8_t)(j<count?d[j]:0u);
  if(v!=stage_screen_db[j]){
   stage_draw_lane(j,v,j<count);
   stage_screen_db[j]=v;
  }
 }
}
static void app_main(void){
 uint8_t st,a,b;uint32_t last_sig=0;uint8_t first_draw=1;smk_pad_events=1;smk_pad_grid=1;palette_set(3);lcd_fill(0,0,240,240,C_BLACK);
 organ_program(5u);P.les=1;P.rev=32;ovp_plate_init();ovp_plate_params(stage_plate_size,stage_plate_tone,stage_plate_pre);
 stage_factory_slots();stage_store_boot();
 /* Publish ready only after every audio dependency is initialized. */
 stage_audio_ready=1u; /* volatile publish: audio ISR sees fully initialized state */
 for(;;){platform_poll();keys_tick();controls();stage_mod_leslie();
 while(midi_get(&st,&a,&b)){uint32_t ch=st&15u;
 switch(st&0xF0u){case 0x90u:organ_midi_note(ch,a,b!=0);break;case 0x80u:organ_midi_note(ch,a,0);break;
 case 0xB0u:organ_cc_ch(ch,a,b);break;case 0xC0u:organ_pc(a);break;}}
 if(stage_store_dirty && !stage_data_locked)stage_store_status=2u;
 if(stage_store_dirty && !fm1_in.keys && quiet() && (stage_save_now || fm1_ms-stage_store_change_ms>=10000u) && (stage_save_now || !stage_store_last_ms || fm1_ms-stage_store_last_ms>=30000u))stage_store_commit();
 if(fm1_ms-leds_ms>=40u){leds_ms=fm1_ms;leds();}
 if(first_draw || (fm1_ms-draw_ms>=75u && stage_sig()!=last_sig)){draw_ms=fm1_ms;last_sig=stage_sig();if(first_draw){first_draw=0;screen_draw();stage_screen_snapshot();}else stage_screen_update();}
 }}
