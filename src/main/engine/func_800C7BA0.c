#include "common.h"
void **FieldEng_GetSlot(void);

extern int D_800E0928;

extern u8 D_800E22D8;
extern u8 D_800E22D9;
extern u8 D_800E22DA;
extern u8 D_800E22DC;
extern u8 D_800E22DD;
extern u8 D_800E22DE;
extern s16 D_800E22E0;
extern s16 D_800E22E2;

extern u8 D_800E2318;
extern u8 D_800E2319;
extern u8 D_800E231A;
extern u8 D_800E231C;
extern u8 D_800E231D;
extern u8 D_800E231E;
extern s16 D_800E2320;
extern s16 D_800E2322;

extern u8 D_800F3498;
extern u8 D_800F3499;
extern u8 D_800F349A;
extern u8 D_800F349C;
extern u8 D_800F349D;
extern u8 D_800F349E;
extern s16 D_800F34A0;
extern s16 D_800F34A2;

extern u8 D_800F34D8;
extern u8 D_800F34D9;
extern u8 D_800F34DA;
extern u8 D_800F34DC;
extern u8 D_800F34DD;
extern u8 D_800F34DE;
extern s16 D_800F34E0;
extern s16 D_800F34E2;

int func_800C7BA0(void) {
    int half_a2;
    register int half_a1 asm("$5");
    int shade_a0;
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot();
    slotData = &D_800E0928;
    *slot = slotData;

    value = 0xBD;
    D_800E22DC = value;
    value = 9;
    half_a2 = 0x80;
    half_a1 = 0x80;
    asm volatile("" : "=r"(half_a2), "=r"(half_a1) : "0"(half_a2), "1"(half_a1));
    D_800E22DD = value;
    value = 0xAE;
    D_800F349C = value;
    value = 7;
    D_800F349D = value;
    value = -0x32;
    shade_a0 = 0x50;
    asm volatile("" : "=r"(shade_a0) : "0"(shade_a0));
    D_800F34A0 = value;
    value = 0x68;
    D_800E231C = value;
    value = -0x3C;
    D_800E2320 = value;
    value = 0x42;
    D_800F34DC = value;
    value = 0x20;
    D_800F34DD = value;
    value = 0x32;
    D_800E22E0 = 0;
    D_800E22E2 = half_a2;
    D_800E22D8 = half_a1;
    D_800E22D9 = half_a1;
    D_800E22DA = half_a1;
    D_800E22DE = 0;
    D_800F34A2 = half_a2;
    D_800F3498 = shade_a0;
    D_800F3499 = shade_a0;
    D_800F349A = shade_a0;
    D_800F349E = 0;
    D_800E231D = 0;
    D_800E2322 = half_a2;
    D_800E2318 = shade_a0;
    D_800E2319 = shade_a0;
    D_800E231A = shade_a0;
    D_800E231E = 0;
    D_800F34E0 = value;
    D_800F34E2 = half_a2;
    D_800F34D8 = half_a1;
    D_800F34D9 = half_a1;
    D_800F34DA = half_a1;
    D_800F34DE = 0;

    return 0;
}
