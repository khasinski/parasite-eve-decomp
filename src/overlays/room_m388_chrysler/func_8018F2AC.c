#include "../room_lib/room_lib.h"

extern char D_80194138[];
extern char D_80194150[];
extern char D_8019415C[];
extern int RoomLib_CloseTarget_8018F338(void *o);

int func_8018F2AC(void *o) {
    int result;

    if (FieldEng_GetStatus() >= 2) {
        result = func_800C251C(o, D_80194150);
        result |= func_800C2758(o, D_80194138, D_8019415C);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F338(o);
    }

    return 0;
}
