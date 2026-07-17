#include "../room_lib/room_lib.h"

extern char D_801953A4[];
extern char D_801953D4[];
extern char D_801953EC[];
extern int RoomLib_CloseTarget_80190344(void *o);

int func_801902B8(void *o) {
    int result;

    if (FieldEng_GetStatus() >= 2) {
        result = func_800C251C(o, D_801953D4);
        result |= func_800C2758(o, D_801953A4, D_801953EC);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_80190344(o);
    }

    return 0;
}
