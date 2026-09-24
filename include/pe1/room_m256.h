#ifndef PE1_ROOM_M256_H
#define PE1_ROOM_M256_H

#include "pe1/field_anim.h"

/* ABI declarations used by the room_m256 effect dispatcher. */
extern int D_800B0E64;
extern u16 D_800E11EA;
extern int D_800E27EC;
extern u16 D_800E2850[];
extern short D_800F336C, D_800F336E, D_800F3372, D_800F3374;
extern u16 D_800F3370;
extern int D_80195EF8;
extern int func_801940B0(int mode, void *state);

extern int func_8006E498(int, int);
extern int func_800D3FD8(void);
extern void func_800D3F64(int, int);

int func_80195320(int mode, u16 *out);

#endif
