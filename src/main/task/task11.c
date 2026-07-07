/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_SceneDataTable0;
extern char *g_TaskNodePool;

int Seq_GetElapsed(void);
int Gpu_CheckDrawStatus(void);

int Task_WaitFrameOrDraw(int **arg0) {
    if ((short)Seq_GetElapsed() == *arg0[0]) {
        return 1;
    }
    if ((Gpu_CheckDrawStatus() << 24) != 0) {
        g_SceneDataTable0 -= 0xC;
        *(int *)(g_TaskNodePool + 0x10) = 1;
    } else {
        return 1;
    }

    return 0;
}

int Task_Noop1(void) {
    return 1;
}

int Task_Noop2(void) {
    return 1;
}
