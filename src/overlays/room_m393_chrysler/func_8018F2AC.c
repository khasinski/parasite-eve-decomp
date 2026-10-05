#include "../room_lib/room_lib.h"

extern char D_80194640[];
extern char D_80194658[];
extern char D_80194664[];
extern int RoomLib_CloseTarget_8018F338(void *o);

int func_8018F2AC(void *o) {
    int result;

    if (FieldEng_GetStatus() >= 2) {
        result = func_800C251C(o, D_80194658);
        result |= func_800C2758(o, D_80194640, D_80194664);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F338(o);
    }

    return 0;
}
