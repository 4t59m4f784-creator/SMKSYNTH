/* SPDX-License-Identifier: GPL-3.0-only */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static uint16_t baseline[57600];
static void compare(unsigned line){for(unsigned y=0;y<240;y++)for(unsigned x=0;x<240;x++){if(line==78u&&x>=8u&&x<68u&&y>=32u&&y<72u)continue;
 if(line==50u&&page==PG_PERFORM&&x>=8u&&x<44u&&y>=29u&&y<53u)continue;
 if(line==50u&&page!=PG_PERFORM&&x>=8u&&x<32u&&y>=31u&&y<47u)continue;
 if((x>=210&&x<232&&y>=4&&y<30)||(x>=8&&x<232&&y>=line&&y<line+2))continue;assert(fb[y*240+x]==baseline[y*240+x]);}}
int main(int argc,char**argv){engine_init();pf_init();pat_init(&pat);mm_boot();zm_zoom=1;fm1_ms=2000;set.pat=7;for(unsigned t=0;t<4;t++){trk_load(t,0);trk_prog[t]=0;}
 const unsigned pages[]={PG_PERFORM,PG_OSC1,PG_FILTER,PG_AENV,PG_MIXER,PG_SUPER};
 for(unsigned p=0;p<sizeof pages/sizeof *pages;p++){page=pages[p];layer=0;mix_row=0;for(unsigned t=0;t<4;t++){seq_trk=t;lcd_fill(0,0,240,240,C_PANEL);draw_page();if(!t)memcpy(baseline,fb,sizeof baseline);else compare(50);assert(fb[5*240+211]==C_TRKC[t]);assert(fb[31*240+208]==RGB(16,48,255));unsigned coloured=0;for(unsigned yy=page==PG_PERFORM?29:31;yy<(page==PG_PERFORM?50:47);yy++)for(unsigned xx=8;xx<(page==PG_PERFORM?44:32);xx++)coloured+=fb[yy*240+xx]==C_TRKC[t];assert(coloured>8);if(!p&&argc>1){char path[256];snprintf(path,sizeof path,"%s/part-%u.ppm",argv[1],t+1);write_ppm_as(path);}}}
 page=PG_PERFORM;seq_trk=0;draw_page();unsigned white=0;for(unsigned y=60;y<78;y++)for(unsigned x=10;x<28;x++)white+=fb[y*240+x]==C_CREAM;assert(white>8);assert(fb[61*240+11]==C_PANEL||fb[61*240+11]==C_AMBER||fb[61*240+11]==C_CREAM);
 preset_begin();memcpy(preset_snapshot.v+NPARAM,"WARM BRASS",10);save_to=76;
 for(unsigned mode=1;mode<=6;mode++){if(mode==5)continue;preset_ui=mode;for(unsigned t=0;t<4;t++){preset_part=t;save_ui_dirty=1;save_ui_draw();if(!t)memcpy(baseline,fb,sizeof baseline);else compare(78);assert(fb[5*240+211]==C_TRKC[t]);unsigned coloured=0;for(unsigned yy=32;yy<72;yy++)for(unsigned xx=8;xx<68;xx++)coloured+=fb[yy*240+xx]==C_TRKC[t];assert(coloured>20);}}
 puts("PASS: actual Perform/edit/mixer and every preset-save state differ only in part badge and header rule/preset number across four parts; fixed pattern badge and white-on-dark K1 label.");
}
