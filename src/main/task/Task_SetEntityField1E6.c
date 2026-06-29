extern char *g_CurrentEntity;

int Task_SetEntityField1E6(int **arg0) {
    *(short *)(g_CurrentEntity + 0x1E6) = *arg0[0];
    return 1;
}
