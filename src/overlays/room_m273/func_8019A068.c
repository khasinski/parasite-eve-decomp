#include "common.h"

extern s32 D_800E27EC;
extern char **D_800F32D0;
extern char **D_800F33E0;
extern u8 D_8019AE68;

void func_80194B5C(void);
s32 func_800CE560(void *arg0, s32 arg1, s32 arg2, void (*arg3)(void));
char *func_800CE610(void *arg0);

s32 func_8019A068(s32 state) {
    char *obj;
    register char *root asm("$2");
    register char *source asm("$3");
    register u8 status asm("$3");

    if (state != 1) {
        if (state >= 2) {
            return 0;
        }
        if (state != 0) {
            return 0;
        }
        D_8019AE68 = 0;
        return func_800CE560(D_800F33E0[2], 8, 5, func_80194B5C);
    }
    if (D_8019AE68 != 0) {
        return 2;
    }

    root = D_800F32D0[2];
    status = root[0xE];
    if (status != 0x10 && status != 8) {
        D_8019AE68 = 1;
        return 2;
    }
    if ((D_800E27EC & 3) != 0) {
        return 0;
    }

    obj = func_800CE610(D_800F33E0[2]);
    if (obj != 0) {
        root = D_800F32D0[2];
        source = *(char **)(root + 0x238);
        *(u16 *)(obj + 0) = *(s32 *)(source + 0x594);
        *(u16 *)(obj + 2) = *(s32 *)(source + 0x598);
        *(u16 *)(obj + 4) = *(s32 *)(source + 0x59C);
    }
    return 0;
}
