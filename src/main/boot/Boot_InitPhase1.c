/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/scene_transition.h"
extern int g_GameState[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;
extern int g_GameStateFlags[];
extern int g_GameStateFlagsWrite[] asm("g_GameStateFlags");

void Menu_OpenStartupMemCardDialog(void);

void Menu_CreateItemUsePanel(int arg0);
extern short D_8009D2A4[];
int Menu_InitBonusPointScreen(int arg0, int arg1, int arg2, int *arg3);
int MenuWidget_HasActiveNodes(void);

int Boot_InitPhase1(void) {
    int *state = g_GameState;
    int *node;
    int flags;
    int cursor;

    if ((state[0] & 0x1000) == 0) {
        node = g_TaskNodePool;
        flags = *(unsigned short *)(node + 2);
        if (flags & 0x20) {
            goto finish;
        }
        *(unsigned short *)(node + 2) = flags | 0x20;
        cursor = g_SceneDataTable0;
        node[4] = 1;
        cursor -= 8;
        g_SceneDataTable0 = cursor;
        return 0;
    }

    return 1;

finish:
    Render_BeginSceneLoad();
    Menu_OpenStartupMemCardDialog();
    {
        register int ret asm("$2") = state[0];
        int *tail_node = g_TaskNodePool;

        int tail_flags;
        asm volatile("" : : "r"(ret), "r"(tail_node));
        ret |= 0x9000;
        state[0] = ret;
        ret = g_GameStateFlags[0];
        tail_flags = *(unsigned short *)(tail_node + 2);
        ret |= 4;
        tail_flags &= 0xFFDF;
        g_GameStateFlagsWrite[0] = ret;
        *(unsigned short *)(tail_node + 2) = tail_flags;
        asm volatile("" : : : "$2", "memory");
        return 1;
    }
}

int Boot_InitPhase2(int **arg0) {
    int *state = g_GameState;
    int **saved = arg0;
    int *node;
    int flags;
    int cursor;

    if (state[0] & 0x1000) {
        goto ret_one;
    }

    node = g_TaskNodePool;
    flags = *(unsigned short *)(node + 2);
    if (flags & 0x20) {
        goto finish;
    }
    *(unsigned short *)(node + 2) = flags | 0x20;
    cursor = g_SceneDataTable0;
    node[4] = 1;
    cursor -= 0xC;
    g_SceneDataTable0 = cursor;
    return 0;

finish:
    Render_BeginSceneLoad();
    Menu_CreateItemUsePanel(**saved);
    {
        register int ret asm("$2") = state[0];
        int *tail_node = g_TaskNodePool;

        int tail_flags;
        asm volatile("" : : "r"(ret), "r"(tail_node));
        ret |= 0x9000;
        state[0] = ret;
        ret = g_GameStateFlags[0];
        tail_flags = *(unsigned short *)(tail_node + 2);
        ret |= 4;
        tail_flags &= 0xFFDF;
        g_GameStateFlagsWrite[0] = ret;
        *(unsigned short *)(tail_node + 2) = tail_flags;
    }

ret_one:
    return 1;
}

int Boot_InitPhase3(int **arg0) {
    int **saved = arg0;
    int *node;
    int flags;
    int cursor;
    int temp;

    {
        int *first_node = g_TaskNodePool;
        if ((*(unsigned short *)(first_node + 2) & 0x20) == 0) {
            goto call_builder;
        }
    }
    goto after_builder;

call_builder:
    {
        asm volatile("" : "=r"(saved) : "0"(saved));
        *saved[3] = Menu_InitBonusPointScreen(*saved[0], *saved[1], *saved[2], saved[4]);
    }

after_builder:
    if (MenuWidget_HasActiveNodes() != 0) {
        int *active_node = g_TaskNodePool;
        node = active_node;
        flags = *(unsigned short *)(node + 2);
        if (flags & 0x20) {
            goto set_pending;
        }
        {
            int *state = g_GameState;
            *(unsigned short *)(node + 2) = flags | 0x20;
            state[0] |= 0x9000;
            Render_BeginSceneLoad();
        }
        goto pop_state;

set_pending:
        g_GameStateFlagsWrite[0] = g_GameStateFlags[0] | 4;

pop_state:
        cursor = g_SceneDataTable0;
        node = g_TaskNodePool;
        cursor -= 0x1C;
        g_SceneDataTable0 = cursor;
        node[4] = 1;
        return 0;
    }

    temp = D_8009D2A4[0];
    if (temp != 0) {
        *saved[3] = temp;
        {
            int *clear_node = g_TaskNodePool;
            register int clear_flags asm("$2") = *(unsigned short *)(clear_node + 2);
            clear_flags &= 0xFFDF;
            *(unsigned short *)(clear_node + 2) = clear_flags;
        }
        return 1;
    }
    return 1;
}
