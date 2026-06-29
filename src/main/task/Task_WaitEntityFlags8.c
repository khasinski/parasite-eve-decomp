/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern char *g_CurrentEntity[];
extern int g_SceneDataTable0;
extern char *g_TaskNodePool;

int Task_WaitEntityFlags8(void) {
    char *state;
    unsigned short flags;

    state = g_TaskNodePool;
    flags = *(unsigned short *)(state + 8);

    if (flags & 0x20) {
        if (*(int *)(g_CurrentEntity[0] + 0x98) & 8) {
            *(unsigned short *)(state + 8) = flags & 0xFFDF;
            return 1;
        }
    } else {
        *(unsigned short *)(state + 8) = flags | 0x20;
    }

    g_SceneDataTable0 -= 8;
    *(int *)(g_TaskNodePool + 0x10) = 1;
    return 0;
}
