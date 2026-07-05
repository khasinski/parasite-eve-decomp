extern char *g_CurrentEntity;

int Task_Noop(void) {
    return 1;
}

int Task_SetEntityField1C(int **arg0) {
    *(int *)(g_CurrentEntity + 0x1C) = *arg0[0];
    return 1;
}
