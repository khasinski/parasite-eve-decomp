#include "../room_lib/scene_finalize.h"

int func_8018F0AC(RoomEnt *arg0) {
    int result;

    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        result = func_800C251C(arg0, D_80190D48);
        result |= func_800C2758(arg0, D_80190D18, D_80190D60);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F140(arg0);
    }

    return 0;
}
