/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/psyq_cd.h"

void Spu_DrainQueueEntry(void)
{
    CdDsReadQueueEntry *entry;
    int i;
    int tag;

    tag = g_CdDsReadQueue[g_CdDsReadIndex].active;
    while (g_CdPendingReadCount > 0) {
        entry = &g_CdDsReadQueue[g_CdDsReadIndex];
        entry->active = 0;
        entry->command = 0;
        for (i = 0; i < 4; i++) {
            entry->payload[i] = 0;
        }
        entry->arg0C = 0;
        entry->arg10 = 0;
        entry->arg14 = 0;

        g_CdDsReadIndex++;
        if (g_CdDsReadIndex >= 8) {
            g_CdDsReadIndex = 0;
        }

        g_CdPendingReadCount--;
        if (g_CdDsReadQueue[g_CdDsReadIndex].active != tag) {
            break;
        }
    }

    g_CdDsReadQueueState = g_CdDsReadIndex;
}
