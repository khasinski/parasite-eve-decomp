#include "../room_lib/room_lib.h"

extern int D_801956D8;
extern int D_801956DC;
extern int D_801956E0;
extern int D_801956E4;
extern int D_801956E8;
extern int D_801956EC;
extern int D_801956F0;
extern int D_801956F4;

int func_801953D0(int mode, int a1, int a2, int a3) {
    if (mode == 1) {
        goto case_1;
    }
    if (mode == 0) {
        goto case_0;
    }
    if (mode == 2) {
        goto case_2;
    }
    goto common;

case_0:
    D_801956D8 = a1;
    D_801956DC = a2;
    D_801956F4 = a3;
    if (a3 != 0) {
        return 0;
    }
    D_801956F4 = 8;
    goto common;

case_1:
    D_801956E0 = a1;
    D_801956E4 = a2;
    D_801956F0 = a3;
    if (a3 != 0) {
        return 0;
    }
    D_801956F0 = 10;
    goto common;

case_2:
    D_801956E8 = a1;

common:
    D_801956EC = a2;
    return 0;
}
