/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_SceneDataTable0;
extern char *g_TaskNodePool;

int Gpu_CheckDrawStatus(void);

int Entity_YieldOnDrawBusy(void) {
    if ((Gpu_CheckDrawStatus() << 24) != 0) {
        g_SceneDataTable0 -= 8;
        *(int *)(g_TaskNodePool + 0x10) = 1;
    } else {
        return 1;
    }

    return 0;
}
