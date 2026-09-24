#ifndef PE1_ROOM_M256_H
#define PE1_ROOM_M256_H

#include "pe1/field_anim.h"

/* ABI declarations used by the room_m256 effect dispatcher. */
extern int D_800B0E64;
extern u16 D_800E11EA;
extern int D_800E27EC;
extern u16 D_800E2850[];
extern u16 D_800F336C;
extern short D_800F336E, D_800F3372, D_800F3374;
extern u16 D_800F3370;
extern int D_80195EF8;
extern int func_801940B0(int mode, void *state);

extern int func_8006E498(int, int);
extern int func_800D3FD8(void);
extern void func_800D3F64(int, int);

int func_80195320(int mode, u16 *out);

typedef struct RoomM256Particle {
    u16 frame;
    u16 offset;
    s16 scale;
} RoomM256Particle;

extern char D_801960A0[];
extern char D_80196098[];
extern char D_80195E64[];
extern s16 D_80196094;
extern s16 D_800F336A;
extern u16 D_800E1204[];
extern int D_800F3428;
extern int *D_800BCFA4;

int func_80077DC4(int);
int func_80077AA4(int, int);
void func_800CF844(void *, void *, int, void *, int, int);
void func_800CF3AC(void *, void *, int);
void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);
void func_800D1DEC(void *, void *, int, int);
int func_8019552C(int mode, RoomM256Particle *particle);

#endif
