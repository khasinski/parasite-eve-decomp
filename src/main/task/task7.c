/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

extern int g_FieldMoveLock[];

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];
extern int g_SceneDataTable0;

int Task_GetPlayerInputFlag(int **arg0) {
    if (g_FieldMoveLock[0] & 1) {
        *arg0[0] = 0;
    } else {
        *arg0[0] = 1;
    }
    return 1;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/task/task7", Task_RunQueue);

int Task_JumpToEntityOffset(int **arg0) {
    g_SceneDataTable0 = g_CurrentEntity[0]->field_9c + (*arg0)[0] * 2;
    return 1;
}
