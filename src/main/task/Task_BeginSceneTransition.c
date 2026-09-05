/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
extern int g_GameState[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;
extern int g_GameStateFlags[];
extern int g_GameStateFlagsWrite[] asm("g_GameStateFlags");

int Render_BeginSceneLoad(void);
void Menu_OpenEquipScreen(int arg0);

int Task_BeginSceneTransition(int **arg0) {
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
    Menu_OpenEquipScreen(**saved);
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
