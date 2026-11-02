/* SPDX-License-Identifier: GPL-3.0-only */
/* KBD1: append-only pattern extension, deliberately absent from scenes. */
enum { KB_SINGLE, KB_SPLIT, KB_LAYER };
typedef struct { uint8_t mode,part[2],split; int8_t octave[2]; uint8_t level[2]; } keyboard_cfg_t;
#define KEYBOARD_MAGIC 0x3144424Bu
static void keyboard_defaults(keyboard_cfg_t *k){*k=(keyboard_cfg_t){KB_SINGLE,{0,1},60,{0,0},{100,100}};}
static int keyboard_valid(const keyboard_cfg_t *k){return k->mode<=KB_LAYER&&k->part[0]<4&&k->part[1]<4&&k->part[0]!=k->part[1]&&k->split>=1&&k->split<=127&&k->octave[0]>=-2&&k->octave[0]<=2&&k->octave[1]>=-2&&k->octave[1]<=2&&k->level[0]<=100&&k->level[1]<=100;}
