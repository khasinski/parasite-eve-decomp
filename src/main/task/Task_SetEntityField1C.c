extern char *g_CurrentEntity;

int Task_SetEntityField1C(int **arg0) {
    *(int *)(g_CurrentEntity + 0x1C) = *arg0[0];
    return 1;
}
