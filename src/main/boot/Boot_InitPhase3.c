/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
extern int g_GameState[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;
extern int g_GameStateFlags;
extern short D_8009D2A4[];

int Menu_InitBonusPointScreen(int arg0, int arg1, int arg2, int *arg3);
int MenuWidget_HasActiveNodes(void);
int Render_BeginSceneLoad(void);

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
        asm volatile(
            "lui $2, %%hi(g_GameStateFlags)\n"
            "lw $2, %%lo(g_GameStateFlags)($2)\n"
            "nop\n"
            "ori $2, $2, 4\n"
            "lui $1, %%hi(g_GameStateFlags)\n"
            "sw $2, %%lo(g_GameStateFlags)($1)\n"
            :
            :
            : "$1", "$2", "memory");

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
