#ifndef PE1_MENU_MEMCARD_DISPLAY_H
#define PE1_MENU_MEMCARD_DISPLAY_H

#include "common.h"
#include "pe1/psyq_gpu.h"

/* Double-buffered display and draw environments used by the video player. */
extern DISPENV D_800BCE80[2];
extern DRAWENV D_800BCDC8[2];

/* Video display mode of each copied player (2 = 320 wide, 3 = 480 wide). */
extern u8 D_801D0DBE;
extern u8 D_801223F6;

DISPENV *func_800749D8(DISPENV *env, s32 x, s32 y, s32 w, s32 h); /* SetDefDispEnv */
DRAWENV *func_80074924(DRAWENV *env, s32 x, s32 y, s32 w, s32 h); /* SetDefDrawEnv */

#endif
