#include "common.h"

extern u8 D_801995D4[];
extern u8 D_80198B90[];
extern u8 D_801995D0, D_801995D1, D_801995D2;
extern u8 D_801995CC, D_801995CD, D_801995CE;
extern u16 D_801995DA, D_801995DC, D_801995D6, D_801995D8;
void func_800C4E50(void *item);

void func_80190F0C(void *unused0, void *unused1, u8 *effect) {
    u8 *mid = D_801995D4;
    u8 *source;
    int width;
    u8 *base;
    int first;
    asm("" : "=r"(mid) : "0"(mid));
    base = mid - 12;
    width = 128;
    first = 300;
    asm volatile("" : : "r"(width));
    *(u16 *)(effect + 8) = first;
    effect[13] = 156;
    effect[12] = 0;
    *(u16 *)(effect + 10) = width;

    *(u16 *)mid = 8;
    D_801995DA = 0;
    D_801995DC = width;
    D_801995D0 = 120;
    D_801995D1 = 100;
    D_801995D2 = 30;
    D_801995CC = 0;
    D_801995CD = 0;
    D_801995CE = 0;
    D_801995D6 = 1800;
    D_801995D8 = 1200;
    source = D_80198B90;
    *(u8 **)(mid - 12) = source;
    func_800C4E50(base);

    *(u16 *)(mid + 24) = 8;
    *(u16 *)(mid + 30) = 0;
    *(u16 *)(mid + 32) = width;
    *(u8 *)(mid + 20) = 20;
    *(u8 *)(mid + 21) = 20;
    *(u8 *)(mid + 22) = 0;
    *(u8 *)(mid + 16) = 120;
    *(u8 *)(mid + 17) = 100;
    *(u8 *)(mid + 18) = 30;
    *(u16 *)(mid + 26) = 1200;
    *(u16 *)(mid + 28) = 800;
    *(u8 **)(mid + 12) = source + 128;
    func_800C4E50(mid + 12);
}
