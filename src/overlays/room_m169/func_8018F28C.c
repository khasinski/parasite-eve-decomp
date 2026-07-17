#include "../room_lib/room_lib.h"

extern char D_80193184[];
extern char D_8019319C[];
extern char D_801931A8[];
extern int RoomLib_CloseTarget_8018F318(void *o);

int func_8018F28C(void *o) {
    int result;

    if (FieldEng_GetStatus() >= 2) {
        result = func_800C251C(o, D_8019319C);
        result |= func_800C2758(o, D_80193184, D_801931A8);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F318(o);
    }

    return 0;
}
