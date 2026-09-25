#include "../room_lib/scene_finalize.h"

int func_8018F0F0(RoomEnt *arg0) {
    int result;

    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        result = func_800C251C(arg0, D_80191A30);
        result |= func_800C2758(arg0, D_80191A00, D_80191A48);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F184(arg0);
    }

    return 0;
}
