#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/room_m023_beacon.h"
#include "pe1/scene_grab_head.h"
#include "pe1/scene_grab_trigger.h"

#include "e19_class_externs.h"

int SceneE19_GrabNop0(void) {
    return 0;
}

s32 SceneE19_GrabInit(SceneGrabTrigger *obj) {
    obj->control.callback = RoomLib_GrabAwaitStance;
    obj->control.started = 0;
    obj->control.grabbed = 0;
    obj->flag3 = 0xFF;
    RotMatrixYXZ(g_SceneE19GrabAngles, (void *)0x1F800028);
    MulMatrix0(g_SceneE19GrabIdentityMatrix, (void *)0x1F800028, (GteMatrix *)&obj->control.player_matrix);
    /* player_matrix.translation, stored as plain words to keep retail order. */
    ((s32 *)obj)[0x30 / 4] = g_SceneE19GrabPlayerTranslation[0];
    ((s32 *)obj)[0x34 / 4] = g_SceneE19GrabPlayerTranslation[1];
    ((s32 *)obj)[0x38 / 4] = g_SceneE19GrabPlayerTranslation[2];
    return 0;
}

int SceneE19_GrabConfigure(SceneGrabTrigger *obj, int mode, int event, int arg3) {
    if (event == 0x19 && mode == 1) {
        obj->control.signal = (int *)arg3;
        *(int *)arg3 = mode;
    }

    return 0;
}

int SceneE19_GrabNop3(void) {
    return 0;
}
