#include "../room_lib/room_lib.h"

extern char D_80192540[];
extern char g_RoomFallingBurstSpawnData[];
extern char g_RoomFallingBurstSpawnScript[];
void RoomLib_CloseTarget_80190674(void *o);

int func_801905E8(void *o) {
    int ret;

    if (FieldEng_GetStatus() >= 2) {
        ret = func_800C251C(o, D_80192540);
        ret = ret | func_800C2758(o, g_RoomFallingBurstSpawnData, g_RoomFallingBurstSpawnScript);
    } else {
        ret = -1;
    }

    if (ret == -1) {
        RoomLib_CloseTarget_80190674(o);
    }

    return 0;
}
