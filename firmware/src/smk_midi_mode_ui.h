/* SPDX-License-Identifier: GPL-3.0-only */
/* Included after the synth's input/panic handlers. */
static void mm_pedal_event(unsigned down){
 if(mm_edit||mm_settings||mm_panic_left)return;
 mm_entry_t e={MM_CC,64,0};
 if(down)mm_press(108,&e,127);else{mm_release(108);mm_touch(108,0);}
}
static void mm_discard_midi(void){uint8_t s,a,b;while(midi_get(&s,&a,&b)){}midi_back[3]=0;}
static void mm_enter(void){
 if(mm_on)return;
 smk_panel_panic();
 fm1_irq_off();mm_on=1;engine_events();smk_synth_controller_reset();fx_used=0;memset(fx_mem,0,sizeof fx_mem);fx_init();dly_lpl=dly_lpr=rv_lp_in=rv_lpl=rv_lpr=rv_yl=rv_yr=0;memset(&m_fl,0,sizeof m_fl);memset(&m_fr,0,sizeof m_fr);m_env=0;m_g=1<<14;fm1_irq_on();
 for(unsigned e=0;e<8;e++)fm1_enc_take(e);
 smk_panic_tx=0; /* controller owns the complete safety flush */
 mm_edit=mm_settings=mm_pick=mm_reset_ask=0;mm_oct=0;mm_kbank=mm_fbank=mm_pbank=0;
 mm_safety();mm_discard_midi();mm_status="READY";layer=layer_lock=0;idle_ok=0;
}
static void mm_leave(void){
 mm_safety();mm_discard_midi();
 mm_edit=mm_settings=mm_pick=mm_reset_ask=0;page=PG_MIDI;layer=layer_lock=0;
 keys_prev=fm1_in.notes;smk_ext_back=-1;
 while(smk_extkey_take()>=0){}while(smk_pad_take()>=0){}
 for(unsigned k=0;k<8;k++)fm1_enc_take(k);
 smk_controls_epoch++;memset(smk_fader_owner,0,sizeof smk_fader_owner);
 smk_pedal.blocked=smk_pedal.down||smk_pedal.candidate;smk_pedal.down=0;ctl_sustain=midi_sustain=0;
 smk_synth_controller_reset();
 mm_wait_release=1;mm_on=0;dirty_hdr=dirty_page=dirty_steps=dirty_tags=1;
 /* Keep the last frame visible until held physical controls are released. */
 scope_state=1;
}
/* A silent/disconnected MIDI host cannot hold the synth UI or local audio.
 * Only outgoing synth notes wait for the still-pending safety flush. */
static void mm_safety_complete(void){if(!mm_on&&!mm_wait_release&&!smk_panic_tx)mout_blocked=0;}
static int mm_return_poll(void){
 if(fm1_in.keys||fm1_in.buttons)return 1;
 mm_wait_release=0;keys_prev=0;mout_blocked=!!(mm_panic_left||smk_panic_tx);
 return 0;
}
static void mm_setting_turn(unsigned k,int e){
 if(!e)return;
 if(!k){if(e>0&&!mm_on)mm_enter();else if(e<0&&mm_on)mm_leave();return;}
 if(!mm_on)return;
 mm_safety();
 if(k==1)mm_data.selected=(uint8_t)clampi((int)mm_data.selected+(e>0?1:-1),0,3);
 else if(k==2)mm_profile()->ch=(uint8_t)clampi((int)mm_main_ch()+e,1,16);
 else mm_profile()->relative=e>0;
 memset(mm_values,0,sizeof mm_values);mm_dirty=1;mm_full_dirty();
}
static void mm_save(void){
 if(fm1_in.keys||mm_pad_down||smk_pedal.down){mm_status="RELEASE KEYS / PEDAL";mm_full_dirty();return;}
 /* Saving an edited map never writes the synth's settings. Alternate CRC
  * sectors; verify before selecting the new generation. Last good survives. */
 mm_safety();unsigned old=mm_data.seq;mm_data.seq++;if(!mm_data.seq)mm_data.seq=1;
 unsigned slot=mm_slot^1u;
 if(mm_store_save(slot,&mm_data,sizeof mm_data)||mm_store_load(slot,&mm_verify,sizeof mm_verify)||memcmp(&mm_verify,&mm_data,sizeof mm_data)){
  mm_data.seq=old;mm_status="SAVE FAILED / RETRY";
 }else{mm_slot=slot;mm_dirty=0;mm_status="SAVED";}
 mm_full_dirty();
}
static void mm_name(unsigned i,char *b){
 b[0]=0;
 if(i<16){scat(b,"KNOB ");scat(b,i<8?"A":"B");fmt_u(b+slen(b),i%8+1);}
 else if(i<MM_PAD){scat(b,"FADER ");scat(b,i<20?"A":"B");fmt_u(b+slen(b),i%4+1);}
 else if(i<MM_BTN){scat(b,"PAD ");scat(b,i<40?"A":"B");fmt_u(b+slen(b),(i-MM_PAD)%16+1);}
 else if(i<MM_PITCH)scat(b,MM_BUTTON_NAME[i-MM_BTN]);
 else scat(b,i==MM_PITCH?"PITCH WHEEL":i==MM_MOD?"MOD WHEEL":"KEYBOARD");
}
static unsigned mm_pad_colour(unsigned i){unsigned c=mm_profile()->color[i-MM_PAD];if(c)return c;unsigned t=mm_profile()->map[i].type;return t==MM_NOTE?(i<40?6:4):t==MM_OFF?9:3;}
static void mm_leds(void){
 for(unsigned b=0;b<18;b++)fm1_led_key(b,0);
 fm1_led_key(BTN_KNOB_BANK,mm_kbank);fm1_led_key(BTN_FADER_BANK,mm_fbank);
 fm1_led_key(BTN_GLO,mm_settings);fm1_led_key(BTN_EDIT,mm_edit);fm1_led_key(BTN_SAVE,mm_dirty&&((fm1_ms/350u)&1u));
 fm1_led_key(BTN_OCT_DOWN,mm_oct<0);fm1_led_key(BTN_OCT_UP,mm_oct>0);
 for(unsigned b=0;b<13;b++)if(!mm_reserved(MM_BTN+b))fm1_led_key(MM_BUTTON[b],mm_toggle[MM_BTN+b]||((mm_buttons_prev>>MM_BUTTON[b])&1u));
 for(unsigned p=0;p<16;p++){
  unsigned i=MM_PAD+16*mm_pbank+p,c=mm_pad_colour(i),active=mm_toggle[i]||(mm_pad_down>>p&1u),level=active?255:80;
  if(mm_profile()->map[i].type==MM_OFF)level=12;
  if(mm_edit&&!mm_pick&&mm_sel==i){c=9;level=(fm1_ms/200u&1u)?230:45;}
  padled_set(p,(uint8_t)(MM_COLOR[c][0]*level/255u),(uint8_t)(MM_COLOR[c][1]*level/255u),(uint8_t)(MM_COLOR[c][2]*level/255u));
 }
 padled_flush();
}
static void mm_select(unsigned i){if(mm_reserved(i))return;mm_sel=i;mm_pick=0;mm_full_dirty();}
static void mm_edit_turn(unsigned k,int e){
 if(!e||mm_reserved(mm_sel))return;
 mm_entry_t *v=&mm_profile()->map[mm_sel];
 if(!k){int t=v->type;do{t=(t+(e>0?1:MM_TYPES-1))%MM_TYPES;}while(!(mm_types(mm_sel)>>t&1u));v->type=(uint8_t)t;}
 else if(k==1)v->num=(uint8_t)clampi((int)v->num+e,0,127);
 else if(k==2)v->ch=(uint8_t)clampi((int)v->ch+e,0,16);
 else if(mm_sel>=MM_PAD&&mm_sel<MM_BTN){uint8_t *c=&mm_profile()->color[mm_sel-MM_PAD];*c=(uint8_t)clampi((int)*c+(e>0?1:-1),0,9);}
 mm_dirty=1;mm_full_dirty();mm_status="EDIT / BT SAVE";
}
static void mui_pads(unsigned y,int selected){
 for(unsigned p=0;p<16;p++){
  unsigned i=MM_PAD+16u*mm_pbank+p,c=mm_pad_colour(i);const uint8_t *rgb=MM_COLOR[c];
  uint16_t color=RGB(rgb[0],rgb[1],rgb[2]);unsigned active=mm_toggle[i]||(mm_pad_down>>p&1u);
  int x=8+(int)(p%8u)*28,yy=(int)y+(int)(p/8u)*24;
  cv_rect(x,yy,23,19,blend565(C_PANEL,color,active?256:100));mui_border(x,yy,23,19,(int)p==selected?C_CREAM:C_RULE);
 }
}
static int mm_ui_black(unsigned n){unsigned s=n%12u;return s==1u||s==3u||s==6u||s==8u||s==10u;}
static unsigned mm_ui_note(unsigned k){unsigned w=mm_owned[71u+k];return w&0x8000u?w&127u:(unsigned)clampi(48+(int)k+12*mm_oct,0,127);}
static void mm_ui_keyboard(uint64_t keys){
 unsigned white=0;
 for(unsigned k=0;k<37;k++)if(!mm_ui_black(k)){int x=10+(int)white++*10;cv_rect(x,201,10,32,C_PANEL);cv_rect(x+1,202,8,30,keys>>k&1u?C_RED:C_CREAM);}
 white=0;for(unsigned k=0;k<37;k++){if(!mm_ui_black(k)){white++;continue;}int x=10+(int)white*10-3;cv_rect(x,201,6,19,keys>>k&1u?C_AMBER:C_PANEL);}
}
static void mm_ui_bars(void){
 zm_text(8,101,"K",16,C_AMBER);
 for(unsigned i=0;i<8;i++){unsigned n=8u*mm_kbank+i;int x=25+(int)i*13,h=(int)(mm_ui_bar[n]*17u/127u);cv_rect(x,98,10,19,C_RULE);if(mm_ui_known>>n&1u)cv_rect(x+2,116-h,6,h,C_AMBER);if(mm_ui_control==n)mui_border(x-1,97,12,21,C_CREAM);}
 zm_text(138,101,"F",16,C_AMBER);
 for(unsigned i=0;i<4;i++){unsigned n=16u+4u*mm_fbank+i;int x=156+(int)i*19,h=(int)(mm_ui_bar[n]*17u/127u);cv_rect(x,98,12,19,C_RULE);if(mm_ui_known>>n&1u)cv_rect(x+3,116-h,6,h,C_AMBER);if(mm_ui_control==n)mui_border(x-1,97,14,21,C_CREAM);}
}
static void mui_live_draw(unsigned parts){
 char name[24],desc[32],value[16],meta[40],n[8],pad[32],notes[24];
 unsigned i=mm_ui_control,v=mm_ui_control_v,relative=i<16u&&mm_relative(),ch=mm_main_ch();
 mm_entry_t e={MM_OFF,0,0};if(i<MM_N){e=mm_profile()->map[i];ch=mm_channel(&e)+1u;mm_name(i,name);}else mui_copy(name,i==108u?"PEDAL":"CONTROLS");
 if(i==108u){e=(mm_entry_t){MM_CC,64,0};v=smk_pedal.down?127u:0u;}
 if(e.type==MM_TOGGLE)v=mm_toggle[i]?127u:0u;
 desc[0]=0;
 if(i==MM_KEYS)mui_copy(desc,"MOVE A CONTROL");
 else{scat(desc,e.type==MM_OFF?"OFF":e.type==MM_BEND?"BEND":e.type==MM_NOTE?"N":"CC");if(e.type!=MM_OFF&&e.type!=MM_BEND){mui_u3(n,e.num);scat(desc,n);}scat(desc," CH");mui_u2(n,ch);scat(desc,n);if(i<16u)scat(desc,relative?" REL":" ABS");}
 if(i==MM_KEYS||e.type==MM_OFF)mui_copy(value,"--");else if(relative)fmt_s(value,(int)v-64);else fmt_u(value,v);
 meta[0]=0;scat(meta,"MAP");fmt_u(meta+slen(meta),mm_data.selected+1u);scat(meta," CH");mui_u2(n,mm_main_ch());scat(meta,n);scat(meta," K:");scat(meta,mm_kbank?"B":"A");scat(meta," F:");scat(meta,mm_fbank?"B":"A");scat(meta," OCT");fmt_s(meta+slen(meta),mm_oct);
 unsigned last=mm_ui_pad,selected=99u;pad[0]=0;
 if(last>=MM_PAD&&last<MM_BTN&&(last-MM_PAD)/16u==mm_pbank){
  selected=(last-MM_PAD)%16u;mm_entry_t pe=mm_profile()->map[last];unsigned pv=pe.type==MM_TOGGLE?(mm_toggle[last]?127u:0u):mm_ui_pad_v;
  mui_u2(pad,selected+1u);scat(pad,pe.type==MM_OFF?" OFF":pe.type==MM_NOTE?" N":" CC");
  if(pe.type!=MM_OFF){fmt_u(pad+slen(pad),pe.num);scat(pad,pe.type==MM_NOTE?" V":" ");fmt_u(pad+slen(pad),pv);}scat(pad," CH");mui_u2(n,mm_channel(&pe)+1u);scat(pad,n);
 }else mui_copy(pad,"TOUCH PAD");
 uint64_t keys=fm1_in.keys&(((uint64_t)1<<37)-1u);unsigned count=0,shown=0;notes[0]=0;
 for(unsigned k=0;k<37;k++)if(keys>>k&1u){count++;if(shown<3u){char note[8];fmt_note(note,mm_ui_note(k));if(shown)scat(notes," ");scat(notes,note);shown++;}}
 if(count>shown){scat(notes," +");fmt_u(notes+slen(notes),count-shown);}if(!count)mui_copy(notes,"--");
 const char *status=mui_footer(meta);
 /* Dirty bands are bounded by the existing 240x120 buffer. Keys use 57 rows,
  * pads 63 and controls 120; a note-off never waits for a whole LCD frame. */
 for(unsigned band=0;band<3;band++){if(!(parts>>band&1u))continue;
  unsigned y=band==0?0:band==1?120:183,h=band==0?120:band==1?63:57;
  cv_begin(240,h,C_PANEL);cv_oy=-(int)y;
  zm_text(8,2,"MIDI MODE",24,C_CREAM);zm_text(176,8,"USB/TRS",16,C_AMBER);zm_text(8,27,status,16,status==meta?C_AMBER:mui_eq(mm_status,"SAVED")?C_CREAM:C_RED);cv_rect(8,45,224,2,C_RED);
  unsigned size=24;while(size>16&&zm_width(name,size)>120)size-=2;zm_text(8,51,name,size,C_CREAM);zm_text(8,78,desc,16,C_AMBER);
  size=slen(value)>3u?32:40;zm_text(232-(int)zm_width(value,size),49,value,size,C_RED);mm_ui_bars();
  cv_rect(8,123,224,1,C_RULE);zm_text(8,127,mm_pbank?"PADS B":"PADS A",16,C_AMBER);zm_text(232-(int)zm_width(pad,16),127,pad,16,C_CREAM);
  for(unsigned p=0;p<16;p++){unsigned source=MM_PAD+16u*mm_pbank+p;const uint8_t*rgb=MM_COLOR[mm_pad_colour(source)];uint16_t c=RGB(rgb[0],rgb[1],rgb[2]);int x=8+(int)(p%8u)*28,yy=145+(int)(p/8u)*19;unsigned active=mm_toggle[source]||(mm_pad_down>>p&1u);cv_rect(x,yy,23,17,blend565(C_PANEL,c,active?256:65));mui_u2(n,p+1u);zm_center(x+12,yy,n,16,C_CREAM);if(p==selected)mui_border(x,yy,23,17,C_CREAM);}
  zm_text(8,183,"KEYS",16,C_AMBER);zm_text(88,183,notes,16,C_CREAM);mm_ui_keyboard(keys);cv_oy=0;cv_blit(0,y);
 }
}

static void mui_edit_draw(void){
 char name[24],number[8],channel[8];mm_name(mm_sel,name);mm_entry_t e=mm_profile()->map[mm_sel];
 mui_u3(number,e.num);if(e.ch)mui_u2(channel,e.ch);else mui_copy(channel,"MAIN");
 unsigned is_pad=mm_sel>=MM_PAD&&mm_sel<MM_BTN,c=is_pad?mm_pad_colour(mm_sel):9u;
 const uint8_t *rgb=MM_COLOR[c];uint16_t color=RGB(rgb[0],rgb[1],rgb[2]);
 const char *type=e.type==MM_TOGGLE?"TOGGLE":e.type==MM_BEND?"BEND":MM_TYPE_NAME[e.type];
 for(unsigned y=0;y<240;y+=120){
  mui_begin(y);
  if(mm_pick){
   mui_header("MIDI EDIT",1);mui_center(120,49,&FONT_L,"TOUCH CONTROL",C_AMBER);cv_rect(8,91,224,47,RGB(19,25,36));mui_border(8,91,224,47,C_RULE);mui_center(120,98,&FONT_L,name,C_CREAM);
   mui_pads(157,is_pad&&((mm_sel-MM_PAD)/16u)==mm_pbank?(int)((mm_sel-MM_PAD)%16u):-1);cv_text(8,224,&FONT_S,"K5 CONTROL",C_DIMT);
  }else{
   mui_edit_header(name);mui_cell(0,1,"TYPE",type,e.type,MM_TYPES-1,C_CREAM,0);
   mui_cell(1,1,e.type==MM_NOTE?"NOTE":e.type==MM_BEND?"--":"CC",e.type==MM_BEND?"--":number,e.num,127,C_CREAM,e.type==MM_BEND);
   mui_cell(2,1,"CHANNEL",channel,e.ch,16,C_CREAM,0);
   mui_cell(3,1,is_pad?"COLOR":"--",is_pad?MM_COLOR_NAME[mm_profile()->color[mm_sel-MM_PAD]]:"--",is_pad?mm_profile()->color[mm_sel-MM_PAD]:0,9,color,!is_pad);
   if(is_pad)cv_rect(133,214,90,3,color);
   cv_text(8,224,&FONT_S,mui_footer("BT SAVE    PARA BACK"),C_DIMT);
  }
  mui_end(y);
 }
}
static void mm_draw(void){
 if(!mm_screen_dirty||fm1_ms-mm_draw_t<40u)return;
 unsigned parts=mm_ui_parts?mm_ui_parts:7u;mm_ui_parts=0;mm_screen_dirty=0;mm_draw_t=fm1_ms;
 if(mm_reset_ask)mui_reset_draw();else if(mm_edit)mui_edit_draw();else if(mm_settings)mui_config_draw();else mui_live_draw(parts);
}
static void mm_input(void){
 uint32_t rel,b=fm1_input_edges(&rel),cur=fm1_in.buttons;
 const unsigned pair=(1u<<BTN_GLO)|(1u<<BTN_PLAY);
 static unsigned panic_pair;
 if((cur&pair)==pair&&!panic_pair){mm_safety();smk_pedal.blocked=1;smk_pedal.down=0;mm_status="MIDI PANIC";panic_pair=1;}
 if(panic_pair){b&=~pair;rel&=~pair;if(!(cur&pair))panic_pair=0;}
 mm_buttons_blocked&=cur;b&=~mm_buttons_blocked;rel&=~mm_buttons_blocked;
 if(mm_reset_ask){
  if(b>>BTN_HOME&1u){mm_reset_ask=0;mm_full_dirty();}
  if(b>>BTN_SAVE&1u){mm_default(mm_profile());mm_dirty=1;mm_reset_ask=0;mm_save();}
 }else{
  if(rel>>BTN_GLO&1u){mm_safety();mm_settings=!mm_settings;mm_edit=mm_pick=0;mm_status=mm_settings?"K1 LEFT: SYNTH":"READY";}
  if(b>>BTN_EDIT&1u){mm_safety();mm_edit=!mm_edit;mm_settings=0;mm_pick=mm_edit;mm_status=mm_edit?"PARA EXIT / BT SAVE":"READY";}
  if(b>>BTN_SAVE&1u)mm_save();
  if(b>>BTN_HOME&1u){if(mm_edit){mm_pick=1;mm_full_dirty();}else if(mm_settings){mm_settings=0;mm_full_dirty();}}
  if(mm_settings&&(b>>BTN_REC&1u)){mm_reset_ask=1;mm_full_dirty();}
 }
 if((b&3u)==3u||(cur&3u)==3u){if(b&3u){mm_pbank^=1;mm_full_dirty();}b&=~3u;}
 else {if(b>>BTN_KNOB_BANK&1u){mm_kbank^=1;mm_abs_reset();mm_full_dirty();}if(b>>BTN_FADER_BANK&1u){mm_fbank^=1;mm_full_dirty();}}
 if(b&((1u<<BTN_OCT_DOWN)|(1u<<BTN_OCT_UP))){
  if(mm_edit){int i=(int)mm_sel;do{i=(i+(b>>BTN_OCT_UP&1u?1:MM_N-1))%MM_N;}while(mm_reserved((unsigned)i));mm_select((unsigned)i);}
  else {mm_oct=clampi(mm_oct+(b>>BTN_OCT_UP&1u?1:-1),-3,3);mm_full_dirty();}
 }
 uint64_t keys=fm1_in.keys;mm_keys_blocked&=keys;uint64_t down=keys&~mm_keys_prev&~mm_keys_blocked,up=mm_keys_prev&~keys;mm_keys_prev=keys;
 if(down||up){mm_screen_dirty=1;mm_ui_parts|=4u;}
 for(unsigned k=0;k<37;k++){
  if(up>>k&1u)mm_release(71+k);
  if((down>>k&1u)&&!mm_edit&&!mm_settings&&!mm_reset_ask){mm_entry_t e={MM_NOTE,(uint8_t)clampi(48+(int)k+12*mm_oct,0,127),0};mm_press(71+k,&e,fm1_in.key_vel[k]?fm1_in.key_vel[k]:100);mm_touch(MM_KEYS,e.num);}
 }
 while(smk_extkey_take()>=0){} /* keys are read once as the complete 37-bit mask */
 int ev;while((ev=smk_pad_take())>=0){unsigned p=((unsigned)ev>>8)&15u,vel=(unsigned)ev&127u,i=MM_PAD+16*mm_pbank+p;
  if(!vel){mm_pad_blocked[p]=0;mm_pad_down&=~(1u<<p);mm_release(MM_PAD+p);mm_release(MM_PAD+16+p);mm_screen_dirty=1;mm_ui_parts|=2u;continue;}
  if(mm_edit){mm_select(i);continue;}
  if(mm_pad_blocked[p]||mm_settings||mm_reset_ask)continue;
  if(!(mm_pad_down>>p&1u)){mm_pad_down|=1u<<p;mm_press(i,&mm_profile()->map[i],vel);}
 }
 for(unsigned k=0;k<8;k++){int e=fm1_enc_take(k);if(!e)continue;unsigned i=8*mm_kbank+k;
  if(mm_reset_ask)continue;
  if(mm_settings){if(k<4)mm_setting_turn(k,e);if(!mm_on)return;}
  else if(mm_edit){if(mm_pick){mm_select(i);}else if(k<4)mm_edit_turn(k,e);else if(k==4){int n=(int)mm_sel;unsigned steps=(unsigned)(e<0?-e:e);if(steps>MM_N)steps=MM_N;while(steps--){do{n=(n+(e>0?1:MM_N-1))%MM_N;}while(mm_reserved((unsigned)n));}mm_select((unsigned)n);}}
  else {if(mm_relative())mm_cv(i,e,1);else{mm_values[i]=(uint8_t)clampi((int)mm_values[i]+mm_abs_delta(i,e),0,127);mm_cv(i,mm_values[i],0);}}
 }
 for(unsigned f=0;f<4;f++){unsigned v=fm1_in.fader[f],i=16+4*mm_fbank+f;if(v==mm_fader_prev[f])continue;mm_fader_prev[f]=(uint8_t)v;if(mm_edit){if(mm_pick)mm_select(i);}else if(!mm_settings&&!mm_reset_ask)mm_cv(i,(int)v,0);}
 for(unsigned j=0;j<13;j++){unsigned i=MM_BTN+j,bit=1u<<MM_BUTTON[j];if(mm_reserved(i))continue;
  if(rel&bit)mm_release(i);
  if(b&bit){if(mm_edit){if(mm_pick)mm_select(i);}else if(!mm_settings&&!mm_reset_ask)mm_press(i,&mm_profile()->map[i],127);}
 }
 mm_buttons_prev=cur;mm_keys_blocked&=keys;
 smk_sustain_poll();
}
static void mm_wheels_poll(void){
 if(fm1_in.frames<300u)return;
 int bend=clampi(smk_pw_bend(&smk_pitch_calibration,fm1_in.wheel[0],fm1_ms)+8192,0,16383);
 static int lo=485,hi=615;int raw=fm1_in.wheel[1];if(raw<lo)lo=raw;if(raw>hi)hi=raw;
 int mod=clampi((hi-raw-12)*127/(hi-lo-12>1?hi-lo-12:1),0,127);
 int value[2]={bend,mod};
 for(unsigned w=0;w<2;w++){
  if(mm_edit&&mm_wheel_prev[w]<0){mm_wheel_prev[w]=value[w];continue;}
  if(value[w]==mm_wheel_prev[w])continue;unsigned first=mm_wheel_prev[w]<0,old_control=mm_ui_control,old_value=mm_ui_control_v;mm_wheel_prev[w]=value[w];unsigned i=MM_PITCH+w;
  if(mm_edit){if(mm_pick)mm_select(i);continue;}
  if(mm_settings||mm_reset_ask)continue;
  mm_entry_t *e=&mm_profile()->map[i];
  if(e->type==MM_BEND){/* coalesced separately from 7-bit CC values */mm_bend_value=(unsigned)value[w];mm_bend_pending=1;mm_touch(i,(unsigned)value[w]);}
  else if(e->type==MM_CC)mm_cv(i,w?value[w]:value[w]>>7,0);
  if(first&&value[w]==(w?0:8192)){mm_ui_control=old_control;mm_ui_control_v=old_value;}
 }
}
static void mm_poll(void){
 mm_input();if(!mm_on)return;
 mm_wheels_poll();mm_service();mm_discard_midi();mm_leds();mm_draw();
}
