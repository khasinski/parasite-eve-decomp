#include "../room_lib/room_lib.h"

extern char D_80192480[];
extern char D_80192448[];
extern char D_8019249C[];
void func_8018F1B0(void *o);

int func_8018F124(void *o) {
    int ret;

    if (FieldEng_GetStatus() == 3) {
        ret = func_800C251C(o, D_80192480);
        ret = ret | func_800C2758(o, D_80192448, D_8019249C);
    } else {
        ret = -1;
    }

    if (ret == -1) {
        func_8018F1B0(o);
    }

    return 0;
}
