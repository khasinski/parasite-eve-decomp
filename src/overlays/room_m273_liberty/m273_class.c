#include "pe1/field_actor.h"
#include "pe1/gte_types.h"
#include "pe1/room_m023_beacon.h"
#include "pe1/scene_grab_head.h"

#include "../room_lib/room_lib.h"
#include "m273_class_externs.h"
#include "room_m273.h"

int RoomM273_GrabNop0(void) {
    return 0;
}

int RoomM273_GrabInit(RoomSelectionState *obj) {
    int ret;

    obj->callback = RoomLib_GrabAwaitStance;
    obj->started = 0;
    obj->placementChecked = 0;
    obj->verticalTicks = 0;
    obj->activated = 0;
    obj->flag3 = 0xFF;
    RotMatrixYXZ(&g_RoomM273GrabInitAngles, (void *)0x1F800028);

    ret = MulMatrix0(&g_RoomM273GrabInitMatrix, (void *)0x1F800028, (GteMatrix *)&obj->matrix);
    /* matrix.translation, stored as plain words to keep retail order. */
    *(int *)&((char *)obj)[0x30] = g_RoomM273GrabInitTranslationX;
    *(int *)&((char *)obj)[0x34] = g_RoomM273GrabInitTranslationY;
    *(int *)&((char *)obj)[0x38] = g_RoomM273GrabInitTranslationZ;
    /* Keep the call result live so GCC allocates the final data load to v1. */
    return ret - ret;
}

int RoomM273_GrabConfigure(void *obj, int mode, int event, int arg3, int index) {
    switch (event) {
    case 0x19:
        if (mode == 1) {
            RW32(obj, 0x10) = arg3;
            *(int *)arg3 = mode;
        }
        break;
    case 0x1D:
        RW16(obj, 0x14) = arg3;
        RW8(obj, 0x4C) = g_RoomM273GrabSelectionByIndex[index];
        break;
    }

    return 0;
}

int RoomM273_GrabNop3(void) {
    return 0;
}
