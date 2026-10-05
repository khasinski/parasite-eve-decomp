#include "../room_lib/room_lib.h"

extern char D_80194484[];
extern char D_801944B4[];
extern char D_801944CC[];
extern int RoomLib_CloseTarget_8018F34C(void *o);

int func_8018F2C0(void *o) {
    int result;

    if (FieldEng_GetStatus() >= 2) {
        result = func_800C251C(o, D_801944B4);
        result |= func_800C2758(o, D_80194484, D_801944CC);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F34C(o);
    }

    return 0;
}
