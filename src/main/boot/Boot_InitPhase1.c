/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
extern int g_GameState[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;
extern int g_GameStateFlags;

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
            "addiu $2, $0, 1\n"
            : "=r"(ret), "=r"(tail_node)
            : "r"(state), "0"(ret), "1"(tail_node)
            : "$1", "$3", "memory");
        return ret;
    }
}
