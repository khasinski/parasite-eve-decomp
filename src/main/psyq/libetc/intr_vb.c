/* ASSEMBLER: GNU */
/* Psy-Q LIBETC INTR_VB.OBJ: startIntrVSync, trapIntrVSync, setIntrVSync, memclrIntrVSync. */
#include "pe1/psyq_api_internal.h"

extern int *D_800956B0;

void memclrIntrVSync(void *ptr, int count);

VSyncCallbackSetter startIntrVSync(void) {
    *D_800956B0 = 0x107;
    g_VSyncCount = 0;
    memclrIntrVSync(g_IntrVSyncCallbackTable, 8);
    InterruptCallback(0, trapIntrVSync);
    return setIntrVSync;
}

void trapIntrVSync(void) {
    int i = 0;
    PsyqInterruptHandler *entry;
    g_VSyncCount = (int)((unsigned int)g_VSyncCount + 1);
    entry = g_IntrVSyncCallbackTable;
    for (; i < 8; i++, entry++) {
        PsyqInterruptHandler callback = *entry;
        if (callback != 0) {
            callback();
        }
    }
}

PsyqInterruptHandler setIntrVSync(unsigned int index, PsyqInterruptHandler callback) {
    PsyqInterruptHandler previous = g_IntrVSyncCallbackTable[index];
    if (callback != previous) {
        g_IntrVSyncCallbackTable[index] = callback;
    }
    return previous;
}

void memclrIntrVSync(void *ptr, int count) {
    int *words = ptr;
    int i = count - 1;

    if (count != 0) {
        do {
            *words = 0;
            i--;
            words++;
        } while (i != -1);
    }
}
