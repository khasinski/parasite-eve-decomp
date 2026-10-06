#include "../room_lib/room_lib.h"
#include "pe1/room_module.h"

int func_8018F12C(void *o) {
    int ret;

    if (FieldEng_GetStatus(o) == 3) {
        ret = func_800C251C(o, g_RoomUpdateList);
        ret = ret | func_800C2758(o, g_RoomInitList, g_RoomSpawnLayout);
    } else {
        ret = -1;
    }

    if (ret == -1) {
        RoomLib_CloseTarget(o);
    }

    return 0;
}
