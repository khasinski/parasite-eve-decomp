/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
extern int g_GameState[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;
extern int g_GameStateFlags[];
extern int g_GameStateFlagsWrite[] asm("g_GameStateFlags");

int Render_BeginSceneLoad(void);
void Menu_OpenStartupMemCardDialog(void);

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
