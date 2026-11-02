#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Verify the private writable bank is outside all declared boot file CRCs."""
import struct,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import fm1_install as I
import fm1pkg_make as M
image,n=I.deinterleave(Path(sys.argv[1]).read_bytes())
e=I._dec(image[0x40:0x90]);off,size=struct.unpack_from('<II',e,8)
flash=bytearray(image[off:off+size]);assert flash[0x55000:0x57000]==b'\xff'*0x2000
assert flash[0x57000:0x9d000]==(Path(__file__).resolve().parents[1]/'factory/bank-SMK16P1.bin').read_bytes()
area=bytearray(flash[0x4000:]);M.sfc(area,0,len(area),0,M.KEY)
def ent(at):
 e=area[at:at+32];assert M.crc16(e[2:])==struct.unpack_from('<H',e)[0]
 return struct.unpack_from('<HIIBBH16s',e,2)
dcrc,_,blk,*_=ent(0)
assert M.crc16(area[32:blk])==dcrc and 0x4000+blk<0x55000
files=[]
for p in [32,64]:
 c,o,z,fl,rs,ix,name=ent(p);assert M.crc16(area[o:o+z])==c
 assert 0x4000+o+z<0x55000;files.append((name.rstrip(b'\xff\0').decode(),hex(0x4000+o+z)))
assert ent(32)[2]==320*1024
c,o,z,*_=ent(blk);assert M.crc16(area[blk+o:blk+o+z-32])==c
assert 0x4000+blk+z<0x55000
c,o,z,*_=ent(blk+32);assert M.crc16(area[blk+o:blk+o+z])==c
assert 0x4000+blk+o+z<0x55000
# Raw private saves/journal do not affect any boot CRC range.
flash[0x55000:0x9d000]=bytes((i*29)&255 for i in range(0x48000))
again=bytearray(flash[0x4000:0x55000]);M.sfc(again,0,len(again),0,M.KEY)
assert again[:blk+64+z]==area[:blk+64+z]
print('PASS: boot entries, app slot 320 KiB, app_area_head CRC, app/cfg_tool/EQ CRCs; all end below restore journal; simulated private saves/journal leave boot files unchanged.',files)
