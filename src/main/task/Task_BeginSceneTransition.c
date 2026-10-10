/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/scene_transition.h"
#include "pe1/task_node.h"
#include "pe1/game_state_types.h"

extern Pe1GameState g_GameState;
extern int g_SceneDataTable0;
extern TaskNode *g_TaskNodePool;
extern int g_GameStateFlags[];
extern int g_GameStateFlagsWrite[] asm("g_GameStateFlags");

void Menu_OpenEquipScreen(int arg0);

int Task_BeginSceneTransition(int **arg0) {
    Pe1GameState *state = &g_GameState;
    int **saved = arg0;
    int *node;
    int flags;
    int cursor;

    if (state->flags & 0x1000) {
        goto ret_one;
    }

    node = (int *)g_TaskNodePool;
    flags = *(unsigned short *)(node + 2);
    if ((flags & 0x20) == 0) {
        *(unsigned short *)(node + 2) = flags | 0x20;
        cursor = g_SceneDataTable0;
        ((TaskNode *)node)->active = 1;
        cursor -= 0xC;
        g_SceneDataTable0 = cursor;
        return 0;
    }

    Render_BeginSceneLoad();
    Menu_OpenEquipScreen(**saved);
    {
        TaskNode *tail_node = g_TaskNodePool;
        int tail_flags;

        state->flags |= 0x9000;
        tail_flags = tail_node->flags;
        g_GameStateFlagsWrite[0] = g_GameStateFlags[0] | 4;
        tail_flags &= 0xFFDF;
        tail_node->flags = tail_flags;
    }

ret_one:
    return 1;
}

void Menu_OpenSaveLoadEntryPoint(int arg0);

int Task_OpenSaveLoadMenu(int **arg0) {
    Pe1GameState *state = &g_GameState;
    int **saved = arg0;
    TaskNode *node;
    int flags;
    int cursor;

    if (state->flags & 0x1000) {
        goto ret_zero;
    }

    node = g_TaskNodePool;
    flags = node->flags;
    if (flags & 0x20) {
        Render_BeginSceneLoad();
        Menu_OpenSaveLoadEntryPoint(**saved);
        {
            TaskNode *tail_node = g_TaskNodePool;
            int tail_flags;

            state->flags |= 0x9000;
            tail_flags = tail_node->flags;
            g_GameStateFlagsWrite[0] = g_GameStateFlags[0] | 4;
            tail_flags &= 0xFFDF;
            tail_node->flags = tail_flags;
        }
    } else {
        cursor = g_SceneDataTable0;
        node->flags = flags | 0x20;
        cursor -= 0xC;
        g_SceneDataTable0 = cursor;
    }

    {
        TaskNode *mark_node = g_TaskNodePool;
        int one = 1;

        mark_node->active = one;
    }

ret_zero:
    return 0;
}
