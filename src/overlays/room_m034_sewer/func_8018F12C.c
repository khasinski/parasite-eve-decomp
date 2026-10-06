#include "../room_lib/room_lib.h"

extern char D_8018FF5C[];
extern char D_8018FF34[];
extern char D_8018FF70[];
#include "pe1/room_module.h"

int func_8018F12C(void *o) {
    int ret;

    if (FieldEng_GetStatus() == 3) {
        ret = func_800C251C(o, D_8018FF5C);
        ret = ret | func_800C2758(o, D_8018FF34, D_8018FF70);
    } else {
        ret = -1;
    }

    if (ret == -1) {
        RoomLib_CloseTarget(o);
    }

    return 0;
}
