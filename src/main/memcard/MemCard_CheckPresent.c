#include "pe1/memcard.h"

void Menu_StepItemGrid2(void);

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
