#include "../room_lib/room_lib.h"

extern char g_RoomFallingBurstSpawnData[];
extern char D_801953D4[];
extern char g_RoomFallingBurstSpawnScript[];
extern int RoomLib_CloseTarget_80190344(void *o);

int func_801902B8(void *o) {
    int result;

    if (FieldEng_GetStatus() >= 2) {
        result = func_800C251C(o, D_801953D4);
        result |= func_800C2758(o, g_RoomFallingBurstSpawnData, g_RoomFallingBurstSpawnScript);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_80190344(o);
    }

    return 0;
}
