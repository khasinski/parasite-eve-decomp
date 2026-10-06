#include "../room_lib/room_lib.h"

extern char g_RoomFallingBurstSpawnData[];
extern char D_801944B4[];
extern char g_RoomFallingBurstSpawnScript[];
extern int RoomLib_CloseTarget_8018F34C(void *o);

int func_8018F2C0(void *o) {
    int result;

    if (FieldEng_GetStatus() >= 2) {
        result = func_800C251C(o, D_801944B4);
        result |= func_800C2758(o, g_RoomFallingBurstSpawnData, g_RoomFallingBurstSpawnScript);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F34C(o);
    }

    return 0;
}
