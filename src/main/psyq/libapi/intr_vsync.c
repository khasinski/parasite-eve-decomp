#include "pe1/psyq_api_internal.h"

extern PsyqInterruptHandler g_IntrVSyncCallbackTable[];

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
