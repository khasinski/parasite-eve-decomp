#include "../room_lib/room_lib.h"
#include "pe1/room_module.h"

int func_8018F374(void *arg0) {
    int result;

    if (FieldEng_GetStatus(arg0) == 3) {
        result = func_800C251C(arg0, g_RoomUpdateList);
        result |= func_800C2758(arg0, g_RoomInitList, g_RoomSpawnLayout);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget(arg0);
    }

    return 0;
}
