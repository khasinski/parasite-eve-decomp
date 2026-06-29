extern short g_CameraBaseAngleX;
extern short g_CameraBaseAngleY;

int Task_SetScreenScrollPos(int **arg0) {
    g_CameraBaseAngleX = *arg0[0];
    asm volatile("" ::: "memory");
    g_CameraBaseAngleY = *arg0[1];
    return 1;
}
