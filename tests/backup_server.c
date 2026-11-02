/* SPDX-License-Identifier: GPL-3.0-only */
#define main restore_test_main
#include "backup_restore_test.c"
#undef main
int main(int argc,char**argv){assert(argc==2);FILE*f=fopen(argv[1],"rb");if(f){assert(fread(flash,1,sizeof flash,f)==sizeof flash);fclose(f);reset();}else initial();char line[1400];while(fgets(line,sizeof line,stdin)){unsigned n=strcspn(line,"\n");assert(!(n&1)&&n/2<=sizeof rx);rx_n=n/2;for(unsigned i=0;i<rx_n;i++){unsigned v;assert(sscanf(line+i*2,"%2x",&v)==1);rx[i]=(uint8_t)v;}tx_n=0;assert(b1_route());for(unsigned i=0;i<tx_n;i++)printf("%02x",tx[i]);puts("");fflush(stdout);}f=fopen(argv[1],"wb");assert(f&&fwrite(flash,1,sizeof flash,f)==sizeof flash);fclose(f);return 0;}
