extern char *g_CurrentEntity;

asm(".globl func_80017948");
asm("func_80017948 = Task_SetCurrentEntityFieldD");

int Task_SetCurrentEntityFieldD(int **arg0) {
    *(char *)(g_CurrentEntity + 0xD) = *arg0[0];
    return 1;
}
