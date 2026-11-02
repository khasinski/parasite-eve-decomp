#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_task_pool.h"

#include "../room_lib/room_lib.h"
#include "m318_player_gate_externs.h"

s32 RoomM318_PlayerGate(s32 arg0, void *arg1, s32 *arg2) {
    FieldActorState *actor;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        FX_TASK_AS(s16)[10] = 0;
        FX_TASK_AS(s16)[11] = 0;
        {
            /* The player battle entity, read as its field actor record. */
            FieldActorState *actor0 = ((FieldActor *)D_8009CD10)->state;
            s32 mask = 0xEFFFFFFF;
            actor0->flags &= mask;
        }
        return 0;
    } else {
        actor = ((FieldActor *)D_8009CD10)->state;
        if (actor->flags & 0x10000000) {
            FX_TASK_AS(s16)[10] = 1;
            FX_TASK_AS(s16)[11] = 1;
            return 1;
        }
        if (g_FieldFxCurrentAge < *arg2) {
            return 0;
        }
        FX_TASK_AS(s16)[10] = 1;
        FX_TASK_AS(s16)[11] = 0;
        return 1;
    }
}

/* A second single-argument script entry; the room's first is the shared
 * RoomLib_SetArg1 unit linked just before this one. */
ROOMLIB_SET_ARG1(RoomM318_SetScriptArg2, g_RoomM318ScriptArg2)

int *RoomM318_GetActorParams(void) {
    return g_RoomM318ActorParams;
}
