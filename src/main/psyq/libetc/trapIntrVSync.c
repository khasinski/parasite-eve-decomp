/* CC1_FLAGS: -mno-split-addresses */
/* GCC_VERSION: 2.8.1 */
#include "pe1/psyq_api_internal.h"

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
