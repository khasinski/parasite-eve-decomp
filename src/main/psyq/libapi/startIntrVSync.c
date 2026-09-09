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
