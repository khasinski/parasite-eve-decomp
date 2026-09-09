#include "pe1/psyq_api_internal.h"

extern void (*g_IntrVSyncCallbackTable[])(void);
extern volatile int g_VSyncCount;
extern int *D_800956B0;

void memclrIntrVSync(void *ptr, int count);
void trapIntrVSync(void);

VSyncCallbackSetter startIntrVSync(void) {
    *D_800956B0 = 0x107;
    g_VSyncCount = 0;
    memclrIntrVSync(g_IntrVSyncCallbackTable, 8);
    InterruptCallback(0, trapIntrVSync);
    return setIntrVSync;
}
