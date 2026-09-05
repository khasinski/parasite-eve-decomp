#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_SceneDataTable0;
extern int *g_TaskNodePool;
extern char *g_CurrentEntity[];
/* Same symbol; keep the post-call address calculation independent in GCC. */
extern char *g_CurrentEntityAfterAction[] asm("g_CurrentEntity");

void Entity_SetActionMode(char *arg0, int arg1);

int Task_SetEntityActionAndWait(int **arg0) {
    char *node = (char *)g_TaskNodePool;
    int flags = *(u16 *)(node + 8);

    if ((flags & 0x20) == 0) {
        int *ptr;
        int mode;

        *(u16 *)(node + 8) = flags | 0x20;
        ptr = arg0[0];
        {
            char *state;
            state = g_CurrentEntity[0];
            mode = *(u16 *)ptr;
            Entity_SetActionMode(state, mode);
        }
        {
            char *state;
            state = g_CurrentEntityAfterAction[0];
            *(int *)(state + 0x98) &= -0x101;
        }
        goto pop_state;
    } else {
        char *state;
        int keep;

        state = g_CurrentEntity[0];
        if (*(u8 *)(state + 0xF) == 0) {
            goto finish;
        }
        if (*(int *)(state + 0x1C) >= 0) {
            register unsigned int lhs asm("$2") = *(unsigned int *)(state + 0x14);
            unsigned int rhs = *(unsigned int *)(state + 0x18);
            int result = lhs < rhs;
            keep = result;
        } else {
            unsigned int rhs = *(unsigned int *)(state + 0x14);
            unsigned int lhs = *(unsigned int *)(state + 0x18);
            int result = lhs < rhs;
            keep = result;
        }
        if (keep == 0) {
            goto pop_state;
        }

finish:
        *(u16 *)(node + 8) = flags & 0xFFDF;
        return 1;
    }

pop_state:
    {
        int cursor = g_SceneDataTable0;
        int *active_node = g_TaskNodePool;
        cursor -= 0xC;
        g_SceneDataTable0 = cursor;
        active_node[4] = 1;
        return 0;
    }
}
