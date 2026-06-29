extern char *g_CurrentEntity;

int Task_ScaleAnimValue(int **arg0) {
    int value = *arg0[0];
    int scale = *(short *)(g_CurrentEntity + 0x224) * 2;
    char *dst = *(char **)(g_CurrentEntity + 0x1B4);

    *(short *)(dst + 0x14) = (scale * value) >> 16;
    return 1;
}
