/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */
extern int g_GameState[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;
extern int g_GameStateFlags;

void Render_BeginSceneLoad(void);
void Menu_OpenSaveLoadEntryPoint(int arg0);

int Task_OpenSaveLoadMenu(int **arg0) {
    register int *state asm("$16") = g_GameState;
    int **saved = arg0;
    int *node;
    int flags;
    int cursor;

    if (state[0] & 0x1000) {
        goto ret_zero;
    }

    node = g_TaskNodePool;
    flags = *(unsigned short *)(node + 2);
    if (flags & 0x20) {
        Render_BeginSceneLoad();
        Menu_OpenSaveLoadEntryPoint(**saved);
        {
            int ret = state[0];
            register int *tail_node asm("$4") = g_TaskNodePool;
            int tail_flags;

            asm volatile(
                "ori $2, $2, 0x9000\n"
                "sw $2, 0($16)\n"
                "lui $2, %%hi(g_GameStateFlags)\n"
                "lw $2, %%lo(g_GameStateFlags)($2)\n"
                "lhu $3, 8($4)\n"
                "ori $2, $2, 4\n"
                "andi $3, $3, 0xFFDF\n"
                "lui $1, %%hi(g_GameStateFlags)\n"
                "sw $2, %%lo(g_GameStateFlags)($1)\n"
                : "=r"(ret), "=r"(tail_node), "=r"(tail_flags)
                : "0"(ret), "1"(tail_node)
                : "$1", "memory");
            *(unsigned short *)(tail_node + 2) = tail_flags;
        }
    } else {
        cursor = g_SceneDataTable0;
        *(unsigned short *)(node + 2) = flags | 0x20;
        cursor -= 0xC;
        g_SceneDataTable0 = cursor;
    }

    {
        int *mark_node = g_TaskNodePool;
        int one = 1;

        mark_node[4] = one;
    }

ret_zero:
    return 0;
}
