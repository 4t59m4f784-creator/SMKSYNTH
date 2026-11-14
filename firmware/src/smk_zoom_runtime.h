/* SPDX-License-Identifier: GPL-3.0-only */
#pragma once
static void zm_mark_dirty(void){dirty_page=1;}
static void zm_focus_capture(unsigned k,int p){
 if(!(zm_zoom||zm_scope_active())||layer||is_seq_page(page)||page==PG_THEME||page==PG_KEYBOARD||page==PG_MIDI||page==PG_CONTROLS)return;
 int numeric=p>=0&&(PDEF[p].kind==K_NUM||PDEF[p].kind==K_BIP||PDEF[p].kind==K_SEMI||PDEF[p].kind>=K_MS);
 if(page==PG_MIXER)numeric=1;if(page==PG_DRIVE)numeric=k!=0;if(page==PG_SUPER)numeric=k<2;
 if(obv_drums_selected()&&page==PG_DSOUND)numeric=k!=0;
 if(numeric){zm_focus_knob=k+1;zm_focus_on=1;zm_focus_t=fm1_ms;}
}
static void zm_scope_rearm(void){unsigned best=0,level=0;for(unsigned v=0;v<NV;v++)if((unsigned)ui_vlevel[v]>level){level=ui_vlevel[v];best=ui_vnote[v];}if(level){uint32_t inc=pitch_inc((int32_t)best<<8),period=inc?0xFFFFFFFFu/inc:1000u;scope_stride=clampi((int32_t)(period*2u/SCOPE_N),1,64);}scope_state=1;}
static int zm_display_poll(void){
 static unsigned scope_was;
 unsigned full=zm_full();uint32_t owner=full?1u+page+(layer<<8)+(zm_scope_active()<<12)+(zm_zoom<<13)+(zm_scope_active()?0u:((seq_trk%NTRK)<<14)+(set.pat<<16)):0u;
 if(owner!=zm_display_owner){zm_display_owner=owner;lcd_fill(0,0,240,240,C_PANEL);dirty_hdr=dirty_page=dirty_tags=dirty_steps=1;scope_state=1;zm_footer_hash=0;zm_scope_t=0;hints_key=0xFFFFFFFFu;}
 if(!full){scope_was=0;return 0;}
 if(zm_focus_on&&!zm_focus_visible()){zm_focus_on=0;dirty_page=1;}
 /* Display-only context notices while the scope remains the active PERFORM view. */
 static int oldmaster=-1;static unsigned oldtempo;
 if(zm_scope_active()&&scope_was){
  char v[24];if(oldmaster>=0&&oldmaster!=smk_master){fmt_u(v,((unsigned)smk_master*100u+511u)/1023u);scat(v,"%");zm_focus_named("VOLUME",v);dirty_page=1;}
  else if(oldtempo&&oldtempo!=set.tempo){fmt_u(v,set.tempo);scat(v," BPM");zm_focus_named("TEMPO",v);dirty_page=1;}
 }
 scope_was=zm_scope_active();
 oldmaster=smk_master;oldtempo=set.tempo;
 static uint32_t ui_t;
 if((dirty_hdr||dirty_page||dirty_tags||dirty_steps)&&fm1_ms-ui_t>=50u){ui_t=fm1_ms;dirty_hdr=dirty_page=dirty_tags=dirty_steps=0;if(zm_transpose_active())draw_seq_knobs();else draw_page();}
 if(zm_scope_active()&&fm1_ms-zm_scope_t>=50u){
  if(scope_state==3u){zm_scope_t=fm1_ms;zm_scope_plot();zm_scope_rearm();}
  else if(fm1_ms-zm_scope_t>=400u){memset(scope,0,sizeof scope);zm_scope_t=fm1_ms;zm_scope_plot();scope_state=1;}
 }
 if(fm1_ms-zm_footer_t>=50u&&page!=PG_THEME){zm_footer_t=fm1_ms;uint32_t h=0;
  if(obv_drums_selected()){for(unsigned i=0;i<8;i++)h=h*7u+(drum_stamp[seq_trk%NTRK][i]&&eng_t+1u-drum_stamp[seq_trk%NTRK][i]<FS/8u);}
  else for(unsigned v=0;v<NV;v++)h=h*9u+((unsigned)ui_vlevel[v]>>11);
  if(h!=zm_footer_hash){zm_footer_hash=h;zm_activity();}
 }
 return 1;
}
static void zm_save_draw(unsigned mode,unsigned waiting,unsigned left){
 char num[8],name[PATCH_NAME+1],sub[32];patch_t target;
 for(unsigned y=0;y<240;y+=120){mui_begin(y);
  if(pb_confirm){mui_u2(num,pb_confirm);sub[0]=0;scat(sub,"PATTERN ");scat(sub,num);scat(sub," / STORED");zm_head("OVERWRITE",sub,'P');
   const psnd_t*p=&psnd[(pb_confirm-1u)%NPAT];
   for(unsigned t=0;t<NTRK;t++){int yy=62+(int)t*32;char l[2]={(char)('A'+t),0};zm_text(8,yy,l,24,C_AMBER);
    if(p->magic==PSN2_MAGIC){mui_u3(num,p->prog[t]+1u);zm_name(name,&p->p[t]);sub[0]=0;scat(sub,num);scat(sub," ");scat(sub,name);zm_text(30,yy+2,num,20,C_TRKC[t]);zm_text(30+(int)zm_width(num,20),yy+2,sub+3,20,C_CREAM);}else zm_text(30,yy+2,"NO STORED SOUND",16,C_DIMT);
   }
   sub[0]=0;scat(sub,"PAD ");fmt_u(sub+4,pb_confirm);scat(sub," CONFIRM ");fmt_u(sub+slen(sub),left);scat(sub,"s");zm_text(8,204,sub,16,C_RED);zm_text(8,223,"OTHER PAD / PATCH CANCEL",16,C_DIMT);
  }else{
   uint16_t c=C_TRKC[preset_part%NTRK];mui_u3(num,save_to+1u);
   zm_text(8,3,mode==1?"SAVE AS":mode==6?"REPLACE?":mode==2?"SAVING":mode==3?"PRESET SAVED":"SAVE FAILED",24,C_CREAM);
   cv_rect(210,4,22,26,c);char part[2]={(char)('A'+preset_part),0};zm_center(221,5,part,24,C_CREAM);
   zm_text(8,32,num,40,c);zm_text(102,45,mode==1?"K7 DEST":mode==6?"DEST":"",20,C_AMBER);cv_rect(8,78,224,2,c);
   if(mode==1||mode==6){
    prog_get(save_to,&target);zm_name(name,&target);zm_text(8,86,"STORED",16,C_AMBER);zm_text(8,105,name,24,C_AMBER);
    zm_text(8,142,"NEW NAME",16,C_DIMT);zm_name(name,&preset_snapshot);zm_text(8,161,name,32,C_CREAM);
    if(mode==1){
     char prefix[PATCH_NAME+1],letter[2]={(char)preset_snapshot.v[NPARAM+preset_cursor],0};memcpy(prefix,preset_snapshot.v+NPARAM,preset_cursor);prefix[preset_cursor]=0;
     cv_rect(8+(int)zm_width(prefix,32),195,(int)zm_width(letter,32),3,C_AMBER);
     char controls[32];controls[0]=0;scat(controls,"K1 POS ");mui_u2(num,preset_cursor+1u);scat(controls,num);scat(controls,"   K2 CHAR ");scat(controls,letter);zm_text(8,203,controls,16,C_DIMT);
     zm_text(8,224,"BT NEXT / PATCH CANCEL",16,C_CREAM);
    }else{zm_text(8,204,"BT CONFIRM",20,C_CREAM);zm_text(8,225,"PATCH BACK",16,C_DIMT);}
   }else{
    zm_name(name,&preset_snapshot);
    if(mode==2){zm_center(120,94,"WAIT",40,C_CREAM);zm_center(120,145,waiting?"STOP / RELEASE":"RELEASE / WAIT",24,C_CREAM);zm_center(120,183,"NOT SAVED YET",16,C_DIMT);zm_center(120,223,"DO NOT SWITCH OFF",16,C_AMBER);}
    else if(mode==3){zm_center(120,100,"SAVED",40,C_CREAM);zm_center(120,156,name,24,C_CREAM);zm_center(120,223,"BACK TO PERFORM",16,C_DIMT);}
    else{zm_center(120,98,"NOT SAVED",32,C_RED);zm_center(120,150,name,24,C_CREAM);zm_center(120,200,"BT RETRY",20,C_AMBER);zm_center(120,225,"PATCH BACK",16,C_DIMT);}
   }
  }mui_end(y);
 }
}
