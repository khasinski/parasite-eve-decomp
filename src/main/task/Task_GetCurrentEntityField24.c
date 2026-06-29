extern char *g_CurrentEntity;

asm(".globl func_80017968");
asm("func_80017968 = Task_GetCurrentEntityField24");

int Task_GetCurrentEntityField24(int **arg0) {
    *arg0[0] = *(unsigned short *)(g_CurrentEntity + 0x24);
    return 1;
}
