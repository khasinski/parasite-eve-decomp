#include "common.h"
extern char D_801928B8;
extern char g_RoomDropFieldSpawnData;
extern char g_RoomDropFieldSpawnScript;

s32 func_800C251C(void *arg0, void *arg1);
s32 func_800C2758(void *arg0, void *arg1, void *arg2);
int FieldEng_GetStatus(void);
#include "pe1/room_module.h"

s32 func_8018F128(void *arg0) {
    s32 ret;

    if (FieldEng_GetStatus() == 3) {
        ret = func_800C251C(arg0, &D_801928B8);
        ret = ret | func_800C2758(arg0, &g_RoomDropFieldSpawnData, &g_RoomDropFieldSpawnScript);
    } else {
        ret = -1;
    }

    if (ret == -1) {
        RoomLib_CloseTarget(arg0);
    }

    return 0;
}
