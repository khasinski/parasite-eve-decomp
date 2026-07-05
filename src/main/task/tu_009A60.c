extern char *g_CurrentEntity;

void Render_ClearObjectAnim(void *arg0);

int Task_ResetEntityRenderObj(void) {
    Render_ClearObjectAnim(g_CurrentEntity + 0x1B4);
    *(int *)(g_CurrentEntity + 0x18C) = 0;
    return 1;
}

int Task_SetEntityField1E6(int **arg0) {
    *(short *)(g_CurrentEntity + 0x1E6) = *arg0[0];
    return 1;
}
