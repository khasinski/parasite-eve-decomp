extern char *g_CurrentEntity;

int Gte_Atan2(int arg0, int arg1);

int Task_ClearEntityRoamFlag(void) {
    *(int *)(g_CurrentEntity + 0x98) &= 0xFFFDFFFF;
    return 1;
}

int Task_SetEntityRoamFlag(void) {
    *(int *)(g_CurrentEntity + 0x98) |= 0x20000;
    return 1;
}

int Task_GetAngleToPoint(int **arg0) {
    int angle;
    int dx;
    int dy;

    dx = (*(int *)(g_CurrentEntity + 0x28) - *arg0[0]) >> 16;
    dy = (*(int *)(g_CurrentEntity + 0x30) - *arg0[1]) >> 16;
    angle = 0x1400 - Gte_Atan2(dy, dx);
    if (angle >= 0x1001) {
        angle -= 0x1000;
    }
    angle -= *(short *)(g_CurrentEntity + 0x3A);
    if (angle < 0) {
        angle += 0x1000;
    }
    *arg0[2] = angle;
    return 1;
}
