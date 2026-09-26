#ifndef MENU_MEMCARD_TRAIL_H
#define MENU_MEMCARD_TRAIL_H

#include "menu_memcard_link.h"

typedef struct MemcardTrailState { s16 phase, timer, attachment; } MemcardTrailState;
extern MemcardPulseState *func_800CE610(void *);
extern u16 D_800E11F6;
extern u8 D_801F1DE8[];

s32 Memcard_TrailEffect(s32 mode, MemcardTrailState *state);

#endif
