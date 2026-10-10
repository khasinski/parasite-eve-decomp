/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/field_actor.h"
#include "pe1/task_node.h"

extern FieldActor *g_CurrentEntity[];
extern TaskNode *g_TaskNodePool;

int Entity_SetCurrentFlags(int **args)
{
    g_CurrentEntity[0]->flags |= (*args)[0];
    return 1;
}

int Entity_GetCurrentNodeId(int **args)
{
    *args[0] = g_TaskNodePool->seq;
    return 1;
}
