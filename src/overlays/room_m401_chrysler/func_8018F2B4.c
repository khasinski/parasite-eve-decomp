#include "../room_lib/room_lib.h"

extern char D_80195314[];
extern char D_8019532C[];
extern char D_80195338[];
extern int RoomLib_CloseTarget_8018F340(void *o);

int func_8018F2B4(void *o) {
    int result;

    if (FieldEng_GetStatus() >= 2) {
        result = func_800C251C(o, D_8019532C);
        result |= func_800C2758(o, D_80195314, D_80195338);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F340(o);
    }

    return 0;
}
