#include "../room_lib/room_lib.h"

extern char D_80192540[];
extern char D_80192510[];
extern char D_80192558[];
void RoomLib_CloseTarget_80190674(void *o);

int func_801905E8(void *o) {
    int ret;

    if (FieldEng_GetStatus() >= 2) {
        ret = func_800C251C(o, D_80192540);
        ret = ret | func_800C2758(o, D_80192510, D_80192558);
    } else {
        ret = -1;
    }

    if (ret == -1) {
        RoomLib_CloseTarget_80190674(o);
    }

    return 0;
}
