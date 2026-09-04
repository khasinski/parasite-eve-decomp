#include "common.h"

extern char D_8019AE9C[];
extern s16 D_8019AEEA;
extern s16 D_8019AEEE;
extern s16 D_8019AEF2;
extern s16 D_8019AEF6;
extern u8 D_8019AEFA;

int func_80052B2C(void);

void *func_8019A1A0(void *unused, int mode) {
    int direction;
    register int value asm("$2");

    D_8019AEFA = mode;
    switch (mode) {
    case 0:
        D_8019AEEE = 0;
        D_8019AEEA = 2;
        break;
    case 1:
        direction = (func_80052B2C() & 1) ? 42 : -42;
        D_8019AEF2 = 2;
        D_8019AEEA = 2;
        PE1_COMPILER_MEMORY_BARRIER();
        value = 0x61;
        D_8019AEEE = direction;
        D_8019AEF6 = value;
        break;
    case 2:
        direction = (func_80052B2C() & 1) ? 0x80 : -0x80;
        D_8019AEF2 = 4;
        D_8019AEEA = 5;
        PE1_COMPILER_MEMORY_BARRIER();
        value = 0xB0;
        D_8019AEEE = direction;
        D_8019AEF6 = value;
        break;
    }
    return D_8019AE9C;
}
