/* SPDX-License-Identifier: GPL-3.0-only
 * Factory DRUM-X/808/909 starting points from JIANT, copyright (C) 2026
 * Leo Kuroshita/Hügelton Instruments and Juan Ignacio Antonucci.
 * 606/707 are synthetic variations applied below; no sample ROM or fidelity claim. */
typedef struct { dx_lane_t lane[8]; uint8_t layer[8]; uint8_t tone,hands; } obv_dkit_t;
static const char *const DRUM_KIT_NAME[]={"808 BOOM","909 CLUB","606 MINI","707 SNAP","DRUM-X"};
static const obv_dkit_t DRUM_KITS[]={
  { {
            {DX_MODE(DXW_SINE, DXF_LP, 1) | DXM_LONG << 5, {3, 40, 102, 0, 6}, {8, 70, 118, 6, 46}},   /* KICK */
            {DX_MODE(DXW_SINE, DXF_BP, 0), {36, 18, 46, 90, 66}, {42, 6, 58, 114, 46}},   /* SNARE */
            {DX_MODE(DXW_SINE, DXF_BP, 0), {56, 0, 50, 127, 70}, {50, 0, 62, 127, 62}},   /* CLAP */
            {DX_MODE(DXW_METAL, DXF_HP, 0), {92, 0, 14, 96, 92}, {100, 0, 6, 112, 98}},   /* HAT CL */
            {DX_MODE(DXW_METAL, DXF_HP, 0), {92, 0, 60, 96, 88}, {100, 0, 72, 112, 94}},   /* HAT OP */
            {DX_MODE(DXW_SINE, DXF_LP, 1) | DXM_LONG << 5, {24, 26, 82, 10, 18}, {30, 46, 92, 20, 38}},   /* TOM */
            {DX_MODE(DXW_BELL, DXF_HP, 1), {65, 10, 14, 16, 38}, {72, 24, 20, 30, 52}},   /* RIM */
            {DX_MODE(DXW_METAL, DXF_BP, 1), {62, 0, 56, 34, 44}, {66, 0, 66, 46, 56}},   /* BELL */
        }, {0, 4, 0, 6, 6, 0, 0, 8}, 0, 8 }, /* 808 BOOM */
  { {
            {DX_MODE(DXW_SINE, DXF_LP, 1) | DX_DRIVE, {7, 90, 68, 4, 50}, {12, 120, 84, 10, 90}},   /* KICK */
            {DX_MODE(DXW_FM, DXF_BP, 0), {40, 34, 60, 92, 104}, {46, 22, 72, 116, 84}},   /* SNARE */
            {DX_MODE(DXW_SINE, DXF_BP, 0), {60, 0, 52, 127, 88}, {54, 0, 64, 127, 80}},   /* CLAP */
            {DX_MODE(DXW_METAL, DXF_HP, 0), {106, 0, 16, 96, 126}, {114, 0, 8, 112, 127}},   /* HAT CL */
            {DX_MODE(DXW_METAL, DXF_HP, 0), {106, 0, 66, 96, 122}, {114, 0, 78, 112, 127}},   /* HAT OP */
            {DX_MODE(DXW_SINE, DXF_LP, 1), {32, 54, 66, 10, 44}, {38, 74, 76, 20, 64}},   /* TOM */
            {DX_MODE(DXW_BELL, DXF_HP, 1), {65, 10, 12, 16, 50}, {72, 24, 18, 30, 64}},   /* RIM */
            {DX_MODE(DXW_METAL, DXF_BP, 1), {64, 0, 44, 34, 50}, {68, 0, 54, 46, 62}},   /* BELL */
        }, {0, 6, 0, 9, 9, 0, 0, 11}, 0, 8 }, /* 909 CLUB */
  { {
            {DX_MODE(DXW_SINE, DXF_LP, 1), {9, 70, 72, 4, 20}, {14, 100, 88, 10, 60}},   /* KICK */
            {DX_MODE(DXW_FM, DXF_BP, 0), {40, 30, 52, 80, 90}, {46, 18, 64, 104, 70}},   /* SNARE */
            {DX_MODE(DXW_SINE, DXF_BP, 0), {60, 0, 46, 127, 84}, {54, 0, 58, 127, 76}},   /* CLAP */
            {DX_MODE(DXW_METAL, DXF_HP, 0), {100, 0, 22, 96, 116}, {108, 0, 14, 112, 122}},   /* HAT CL */
            {DX_MODE(DXW_METAL, DXF_HP, 0), {100, 0, 62, 96, 112}, {108, 0, 74, 112, 118}},   /* HAT OP */
            {DX_MODE(DXW_SINE, DXF_LP, 1), {30, 40, 66, 10, 30}, {36, 60, 76, 20, 50}},   /* TOM */
            {DX_MODE(DXW_BELL, DXF_HP, 1), {63, 10, 14, 16, 46}, {70, 24, 20, 30, 60}},   /* RIM */
            {DX_MODE(DXW_METAL, DXF_BP, 1), {64, 0, 44, 34, 50}, {68, 0, 54, 46, 62}},   /* BELL */
        }, {0, 6, 0, 9, 9, 0, 0, 11}, 0, 4 }, /* 606 MINI */
  { {
            {DX_MODE(DXW_SINE, DXF_LP, 1), {9, 70, 72, 4, 20}, {14, 100, 88, 10, 60}},   /* KICK */
            {DX_MODE(DXW_FM, DXF_BP, 0), {40, 30, 52, 80, 90}, {46, 18, 64, 104, 70}},   /* SNARE */
            {DX_MODE(DXW_SINE, DXF_BP, 0), {60, 0, 46, 127, 84}, {54, 0, 58, 127, 76}},   /* CLAP */
            {DX_MODE(DXW_METAL, DXF_HP, 0), {100, 0, 22, 96, 116}, {108, 0, 14, 112, 122}},   /* HAT CL */
            {DX_MODE(DXW_METAL, DXF_HP, 0), {100, 0, 62, 96, 112}, {108, 0, 74, 112, 118}},   /* HAT OP */
            {DX_MODE(DXW_SINE, DXF_LP, 1), {30, 40, 66, 10, 30}, {36, 60, 76, 20, 50}},   /* TOM */
            {DX_MODE(DXW_BELL, DXF_HP, 1), {63, 10, 14, 16, 46}, {70, 24, 20, 30, 60}},   /* RIM */
            {DX_MODE(DXW_METAL, DXF_BP, 1), {64, 0, 44, 34, 50}, {68, 0, 54, 46, 62}},   /* BELL */
        }, {0, 6, 0, 9, 9, 0, 0, 11}, 0, 8 }, /* 707 SNAP */
  { {
            {DX_MODE(DXW_SINE, DXF_LP, 1), {9, 70, 72, 4, 20}, {14, 100, 88, 10, 60}},   /* KICK */
            {DX_MODE(DXW_FM, DXF_BP, 0), {40, 30, 52, 80, 90}, {46, 18, 64, 104, 70}},   /* SNARE */
            {DX_MODE(DXW_SINE, DXF_BP, 0), {60, 0, 46, 127, 84}, {54, 0, 58, 127, 76}},   /* CLAP */
            {DX_MODE(DXW_METAL, DXF_HP, 0), {100, 0, 22, 96, 116}, {108, 0, 14, 112, 122}},   /* HAT CL */
            {DX_MODE(DXW_METAL, DXF_HP, 0), {100, 0, 62, 96, 112}, {108, 0, 74, 112, 118}},   /* HAT OP */
            {DX_MODE(DXW_SINE, DXF_LP, 1), {30, 40, 66, 10, 30}, {36, 60, 76, 20, 50}},   /* TOM */
            {DX_MODE(DXW_BELL, DXF_HP, 1), {63, 10, 14, 16, 46}, {70, 24, 20, 30, 60}},   /* RIM */
            {DX_MODE(DXW_METAL, DXF_BP, 1), {64, 0, 44, 34, 50}, {68, 0, 54, 46, 62}},   /* BELL */
        }, {0, 6, 0, 9, 9, 0, 0, 11}, 0, 8 }, /* DRUM-X */
};
#define NDRUMKIT ((uint32_t)(sizeof DRUM_KITS/sizeof DRUM_KITS[0]))

