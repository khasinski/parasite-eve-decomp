#include "common.h"
/* MASPSX_FLAGS: --expand-div */
int GetTPage(int tp, int abr, int x, int y);

extern u8 D_800F33AC;
extern u8 D_800E224C;
extern s16 D_800F341C;
extern s16 D_800F341E;
extern u8 D_800F3422;
extern u16 D_800F3424;
extern u16 D_800F3426;
extern u16 D_800E27AC;

void func_800C2EAC(u8 mode) {
    if (mode == 0) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1D7;
        D_800F3422 = 0;
    }
    if (mode == 1) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x160;
        D_800F341E = 0x1DB;
        D_800F341C = 0;
        D_800F3422 = 0x60;
    }
    if (mode == 2) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1D6;
        D_800F3422 = 0;
    }
    if (mode == 3) {
        D_800F3424 = 0x380;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1C8;
        D_800F3422 = 0;
    }

    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}
extern u8 D_800F345C;
extern u8 D_800F345D;
extern s16 D_800F3310;
extern s16 D_800F3312;
extern s16 D_800F3314;
extern s16 D_800F3318;
extern s16 D_800F331A;
extern s16 D_800F331C;
extern s16 D_800F3320;
extern s16 D_800F3322;
extern s16 D_800F3324;
extern s16 D_800F3328;
extern s16 D_800F332A;
extern s16 D_800F332C;

void func_800C2FF0(int width, int height) {
    int widthMinus;
    int heightMinus;
    int x;
    int y;
    widthMinus = width - 1;
    heightMinus = height - 1;
    asm("" : : "r"(widthMinus), "r"(heightMinus));

    D_800F345C = width;
    x = (width & 0xFF) << 4;
    D_800F345D = height;
    y = (height & 0xFF) << 4;

    D_800F3310 = -x;
    D_800F3312 = -y;
    D_800F3314 = 0;
    D_800F3318 = x;
    D_800F331A = -y;
    D_800F331C = 0;
    D_800F3320 = -x;
    D_800F3322 = y;
    D_800F3324 = 0;
    D_800F3328 = x;
    D_800F332A = y;
    D_800F332C = 0;

    D_800F345C = widthMinus;
    D_800F345D = heightMinus;
}
int GetTPage(int tp, int abr, int x, int y);
int printf(const char *fmt, ...);

extern char D_800C2110[];
extern u8 D_800F33AC;
extern u8 D_800E224C;
extern u16 D_800F3424;
extern u16 D_800F3426;
extern u16 D_800E27AC;

void func_800C3098(int mode) {
    mode = (short)mode;

    if (mode == 0x10) {
        goto mode16;
    }
    if (mode == 0x100) {
        goto mode256;
    }
    goto badMode;

mode16:
    D_800F33AC = 0;
    goto done;

mode256:
    D_800F33AC = 1;
    goto done;

badMode:
    printf(D_800C2110);

done:
    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}

void func_800C3134(u8 *table, u32 step, u8 *out) {
    u32 sum = 0;
    u32 one = 0x1000;
    u32 last = 0xFF;
    u8 *entry = table;

    while (1) {
        u32 duration = entry[3];
        u8 *next = entry + 4;

        sum += duration;
        if (step < sum) {
            u32 weight = ((sum - step) << 12) / duration;
            u32 inv = one - weight;
            u32 out0;
            u32 out1;
            u32 out2;

            out0 = ((inv * entry[4]) + (weight * entry[0])) >> 12;
            out1 = ((inv * entry[5]) + (weight * entry[1])) >> 12;
            out2 = ((inv * entry[6]) + (weight * entry[2])) >> 12;
            out[0] = out0;
            out[1] = out1;
            out[2] = out2;
            return;
        }

        if (entry[7] == last) {
            out[0] = entry[4];
            out[1] = entry[5];
            out[2] = entry[6];
            return;
        }

        entry = next;
    }
}
int GetTPage(int tp, int abr, int x, int y);

extern u8 D_800F33AC;
extern u8 D_800E224C;
extern u8 D_800F33B8;
extern u8 D_800F337A;
extern u16 D_800F3424;
extern u16 D_800F3426;
extern u16 D_800E27AC;
extern void *jtbl_800C2128[];

void func_800C3238(u8 mode) {
    D_800F33B8 = mode;

    asm("" : : "X"(&&case0), "X"(&&case1), "X"(&&case2), "X"(&&case3), "X"(&&case4));

    if (mode < 5) {
        goto *jtbl_800C2128[mode];
    }
    goto done;

case0:
        D_800F337A = 0;
        D_800E224C = 0;
        goto done;
case1:
        D_800F337A = 1;
        D_800E224C = 0;
        goto done;
case2:
        D_800F337A = 1;
        D_800E224C = 1;
        goto done;
case3:
        D_800F337A = 1;
        D_800E224C = 2;
        goto done;
case4:
        D_800F337A = 1;
        D_800E224C = 3;

done:
    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}
