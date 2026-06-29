/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned short u16;
typedef unsigned char u8;

extern int g_SceneDataTable0;
extern int *g_TaskNodePool;
extern char *g_CurrentEntity;

void Entity_SetActionMode(char *arg0, int arg1);

#define LOAD_FIELD_STATE(dst)                                         \
    asm volatile(                                                     \
        "lui\t%0, %%hi(g_CurrentEntity)\n"                                 \
        "lw\t%0, %%lo(g_CurrentEntity)(%0)"                                \
        : "=r"(dst))

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
            LOAD_FIELD_STATE(state);
            mode = *(u16 *)ptr;
            Entity_SetActionMode(state, mode);
        }
        {
            char *state;
            LOAD_FIELD_STATE(state);
            *(int *)(state + 0x98) &= -0x101;
        }
        goto pop_state;
    } else {
        char *state;
        int keep;

        LOAD_FIELD_STATE(state);
        if (*(u8 *)(state + 0xF) == 0) {
            goto finish;
        }
        if (*(int *)(state + 0x1C) >= 0) {
            register unsigned int lhs asm("$2") = *(unsigned int *)(state + 0x14);
            unsigned int rhs = *(unsigned int *)(state + 0x18);
            int result = lhs < rhs;
            keep = result;
        } else {
            register unsigned int rhs asm("$3") = *(unsigned int *)(state + 0x14);
            register unsigned int lhs asm("$2") = *(unsigned int *)(state + 0x18);
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
        register int cursor asm("$3") = g_SceneDataTable0;
        int *active_node = g_TaskNodePool;
        cursor -= 0xC;
        g_SceneDataTable0 = cursor;
        active_node[4] = 1;
        return 0;
    }
}
