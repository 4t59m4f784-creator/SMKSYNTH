/* SPDX-License-Identifier: GPL-3.0-only */
/* Adapted from SMKTRON mel.c, ba5d498716e67a8ccb0a5c12f08df608819f2fb2.
 * Four approximate stock SMK-37 ADC bands, not a charge percentage.
 * Source there: genkigenki/fm1-to-smk37 docs/01-hardware.md section 6.6,
 * 54252ffa2219e3c9cd7eb70f4e99b361eaa048e1.
 * Cached ADC only: no conversion, flash write, audio work or charging inference.
 * One-second IIR; a new filtered band needs three consecutive samples.
 * Zero or saturated ADC means unknown (empty outline), never full. */
#pragma once
#include <stdint.h>
typedef struct {uint32_t level,ms,count,average,pending;} obv_battery_t;
static obv_battery_t obv_battery;
static uint32_t obv_battery_poll(uint32_t now,uint32_t raw)
{
 uint32_t old=obv_battery.level,value,next;
 if(now-obv_battery.ms<1000u)return 0;
 obv_battery.ms=now;
 if(!raw||raw>=1023u){
  obv_battery.count=obv_battery.average=obv_battery.pending=obv_battery.level=0;
 }else{
  obv_battery.average=obv_battery.average?(obv_battery.average*7u+(raw<<3))/8u:raw<<3;
  value=obv_battery.average>>3;
  next=1u+(value>=530u)+(value>=560u)+(value>=602u);
  if(next!=obv_battery.pending){obv_battery.pending=next;obv_battery.count=0;}
  if(next!=old&&++obv_battery.count>=3u)obv_battery.level=next;
 }
 return old!=obv_battery.level;
}
