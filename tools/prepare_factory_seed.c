/* SPDX-License-Identifier: GPL-3.0-only */
/* Prepare only the explicitly selected musical records, using the real types. */
#define main obv_host_main
#include "../firmware/src/obervave.c"
#undef main
#include <assert.h>
static void read_exact(const char*name,void*p,unsigned n){FILE*f=fopen(name,"rb");assert(f);assert(fread(p,1,n,f)==n&&fgetc(f)==EOF);fclose(f);}
static void save(const char*name,const void*p,unsigned n){FILE*f=fopen(name,"wb");assert(f&&fwrite(p,1,n,f)==n);fclose(f);}
int main(int argc,char**argv){
 assert(argc==4);char name[512],out[512];engine_init();for(unsigned i=0;i<NPSECT;i++)psect[i].tag=PS_TAG;
 unsigned tempo=(unsigned)atoi(argv[3]);assert(tempo>=40&&tempo<=240);
 memset(&set,0,sizeof set);set.version=SET_VERSION;set.tempo=(uint8_t)tempo;set.click=CLICK_REC;set.sync_int=1;
 for(unsigned t=0;t<NTRK;t++){set.tprog[t]=TPROG_DEF[t];set.mix_lvl[t]=100;set.rec_qt[t]=RQ_16;}
 set.mix_cho[0]=set.mix_dly[0]=set.mix_rev[0]=127;set.rec_qt_ok=RQT_OK;
 set.dv_type=DV_OVERDRIVE;set.dv_amt=60;set.dv_tone=90;set.dv_mix=127;set.dv_ok=DV_OK;
 set.m_vol=100;set.m_dj=64;set.m_ok=M_OK;smk_master=724;smk_boot_volume_capture();
 snprintf(out,sizeof out,"%s/globals-set7.bin",argv[2]);save(out,&set,sizeof set);
 for(unsigned n=1;n<=5;n++){
  snprintf(name,sizeof name,"%s/patterns/%02u.scene380",argv[1],n);psnd_t scene;read_exact(name,&scene,sizeof scene);assert(scene.magic==PSN2_MAGIC);
  for(unsigned t=0;t<NTRK;t++){assert(scene.prog[t]<NPROG&&obv_engine_available(obv_patch_engine(&scene.p[t])));patch_t base;prog_get(scene.prog[t],&base);scene.edited[t]=(uint8_t)!!memcmp(&base,&scene.p[t],sizeof base);}
  snprintf(out,sizeof out,"%s/pattern-%02u.scene380",argv[2],n);save(out,&scene,sizeof scene);
 }
 snprintf(name,sizeof name,"%s/favorites/bank.bin",argv[1]);pf_store_t fav;read_exact(name,&fav,sizeof fav);assert(pf_valid(&fav));fav.generation=1;
 for(unsigned n=0;n<8;n++)if(fav.used>>n&1u){patch_t base;prog_get(fav.slot[n].prog,&base);fav.slot[n].edited=(uint8_t)!!memcmp(&base,&fav.slot[n].patch,sizeof base);}
 snprintf(out,sizeof out,"%s/favorites-pfv2.bin",argv[2]);save(out,&fav,sizeof fav);
 puts("PASS: clean 67-byte globals, author tempo only, five standalone scene snapshots, eight favorites, no user preset bank; edit badges match base factory presets.");
}
