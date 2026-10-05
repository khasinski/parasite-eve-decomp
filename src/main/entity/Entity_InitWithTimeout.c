/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/scene_transition.h"
#include "pe1/task_node.h"
extern int g_SceneDataTable0;
extern TaskNode *g_TaskNodePool;
extern unsigned short D_8009D2A4[];

void Menu_OpenEquipSlotSwap(int arg0);

int Entity_InitWithTimeout(int **arg0) {
    TaskNode *state = g_TaskNodePool;
    unsigned short flags = state->flags;

    if ((flags & 0x20) == 0) {
        state->flags = flags | 0x20;
        Render_BeginSceneLoad();
        g_SceneDataTable0 -= 0x10;
        g_TaskNodePool->active = 1;
        return 0;
    }

    if ((unsigned short)(D_8009D2A4[0] - 3) < 0x180) {
        *arg0[1] = (short)D_8009D2A4[0] - 3;
        g_TaskNodePool->flags &= 0xFFDF;
        return 1;
    }

    if ((short)D_8009D2A4[0] == -1) {
        *arg0[1] = -1;
        g_TaskNodePool->flags &= 0xFFDF;
        return 1;
    }

    Menu_OpenEquipSlotSwap(*arg0[0]);
    g_SceneDataTable0 -= 0x10;
    g_TaskNodePool->active = 1;
    return 0;
}
