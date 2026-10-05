/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

typedef struct Entity Entity;

extern Entity *g_CurrentEntity[];
extern int g_SceneDataTable0;
extern TaskNode *g_TaskNodePool;

int Entity_TriggerAnimEvent(Entity *arg0, unsigned char arg1);

int Task_RunEntityCommand(int **arg0) {
    int result;
    register int active asm("$5");
    register int stack asm("$4");

    result = (signed char)Entity_TriggerAnimEvent(g_CurrentEntity[0], *(unsigned char *)arg0[0]);
    *arg0[1] = result;
    if (*arg0[1] != 0) {
        return 1;
    }

    active = 1;
    stack = g_SceneDataTable0;
    stack -= 0x10;
    g_TaskNodePool->active = active;
    g_SceneDataTable0 = stack;
    return 0;
}
