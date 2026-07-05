
#include "include_asm.h"
extern char *g_CurrentEntity;

asm(".globl func_80017928");
asm("func_80017928 = Task_GetCurrentEntityFieldD");

asm(".globl func_80017948");
asm("func_80017948 = Task_SetCurrentEntityFieldD");

asm(".globl func_80017968");
asm("func_80017968 = Task_GetCurrentEntityField24");

int Task_GetCurrentEntityFieldD(int **arg0) {
    *arg0[0] = *(unsigned char *)(g_CurrentEntity + 0xD);
    return 1;
}

int Task_SetCurrentEntityFieldD(int **arg0) {
    *(char *)(g_CurrentEntity + 0xD) = *arg0[0];
    return 1;
}

int Task_GetCurrentEntityField24(int **arg0) {
    *arg0[0] = *(unsigned short *)(g_CurrentEntity + 0x24);
    return 1;
}
