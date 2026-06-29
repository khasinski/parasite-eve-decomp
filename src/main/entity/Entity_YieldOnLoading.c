/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern unsigned char g_ScreenTransitionState[];
extern int g_SceneDataTable0;
extern char *g_TaskNodePool;

int Entity_YieldOnLoading(void) {
    if ((g_ScreenTransitionState[0] & 3) >= 2) {
        g_SceneDataTable0 -= 8;
        *(int *)(g_TaskNodePool + 0x10) = 1;
        return 0;
    }

    return 1;
}
