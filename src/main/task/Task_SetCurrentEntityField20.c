extern char *g_CurrentEntity;

asm(".globl func_800182E0");
asm("func_800182E0 = Task_SetCurrentEntityField20");

int Task_SetCurrentEntityField20(int **arg0) {
    *(int *)(g_CurrentEntity + 0x20) = *arg0[0];
    return 1;
}
