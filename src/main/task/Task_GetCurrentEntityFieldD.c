extern char *g_CurrentEntity;

asm(".globl func_80017928");
asm("func_80017928 = Task_GetCurrentEntityFieldD");

int Task_GetCurrentEntityFieldD(int **arg0) {
    *arg0[0] = *(unsigned char *)(g_CurrentEntity + 0xD);
    return 1;
}
