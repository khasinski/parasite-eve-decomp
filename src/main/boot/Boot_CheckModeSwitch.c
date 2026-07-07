/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
extern int g_RenderStateFlags[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;

int Boot_CheckModeSwitch(void) {
    int mode = g_RenderStateFlags[0] & 7;

    if (mode == 4) {
        goto ret_one;
    }
    if (mode != 0) {
        goto pop_state;
    }

ret_one:
    return 1;

pop_state:
    {
        int cursor = g_SceneDataTable0;
        int *node = g_TaskNodePool;
        cursor -= 8;
        g_SceneDataTable0 = cursor;
        node[4] = 1;
        return 0;
    }
}
