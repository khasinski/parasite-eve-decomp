#include "pe1/memcard.h"

void Menu_StepItemGrid2(void);

extern int g_MemCardActivePortOneBased;

int MemCard_CheckPresent(int port) {
    int present;

    present = g_MemCardPortStates[port].present & 4;
    if (present != 0) {
        if (g_MemCardActivePortOneBased == 0) {
            g_MemCardActivePortOneBased = port + 1;
            Menu_StepItemGrid2();
        }
    }
    return present == 0;
}

int MemCard_GetActivePort(void)
{
    return g_MemCardActivePortOneBased - 1;
}
