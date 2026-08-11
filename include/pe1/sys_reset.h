#ifndef PE1_SYS_RESET_H
#define PE1_SYS_RESET_H

#include "common.h"

extern s32 D_8010DA0C[];
extern s32 D_8010DA10[16];
extern s32 D_8010DA50[16];
extern s32 D_8010DA90[];

void func_8010C1EC(s32 *data, u32 size);

#endif
