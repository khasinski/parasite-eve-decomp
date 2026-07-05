/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */
extern int g_GameState[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;
extern int g_GameStateFlags;

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
        int ret = state[0];
        register int *tail_node asm("$4") = g_TaskNodePool;

        asm volatile(
            "ori $2, $2, 0x9000\n"
            "sw $2, 0(%2)\n"
            "lui $2, %%hi(g_GameStateFlags)\n"
            "lw $2, %%lo(g_GameStateFlags)($2)\n"
            "lhu $3, 8($4)\n"
            "ori $2, $2, 4\n"
            "andi $3, $3, 0xFFDF\n"
            "lui $1, %%hi(g_GameStateFlags)\n"
            "sw $2, %%lo(g_GameStateFlags)($1)\n"
            "sh $3, 8($4)\n"
            : "=r"(ret), "=r"(tail_node)
            : "r"(state), "0"(ret), "1"(tail_node)
            : "$1", "$3", "memory");
    }

ret_one:
    return 1;
}
