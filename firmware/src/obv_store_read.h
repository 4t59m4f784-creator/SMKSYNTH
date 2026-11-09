/* SPDX-License-Identifier: GPL-3.0-only */
/* Keep the established raw SPI path/encryption policy. Shorter SMK transfers
 * reduce the byte payload per masked interval eightfold; SPI setup still costs time. */
static int st_read(uint32_t off,void*dst,uint32_t n){
 uint8_t*d=dst;
 while(n){
#if BOARD_SMK37
  uint32_t k=n>32u?32u:n;
#else
  uint32_t k=n>256u?256u:n;
#endif
  uint32_t f=irq_save();int rc=FL_FAR(fl_read_ram)(off,d,k);irq_restore(f);
  if(rc)return rc;off+=k;d+=k;n-=k;
 }
 return 0;
}
