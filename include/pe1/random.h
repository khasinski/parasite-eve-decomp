#ifndef PE1_RANDOM_H
#define PE1_RANDOM_H

#include "common.h"

/* Byte generator cursor and its complete 521-byte state. */
extern s32 D_8009D038;
extern u8 D_800A1B90[521];

int rand(void);

int Engine_Random(void) __asm__("func_80071A54");

#endif
