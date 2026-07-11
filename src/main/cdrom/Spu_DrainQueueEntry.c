/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/psyq_cd.h"

extern CdDsReadQueueEntry g_CdDsReadQueue[] __asm__("D_800A3540");
extern int g_CdDsReadIndex __asm__("D_800A3600");
extern int g_CdDsReadQueueState __asm__("D_800A3604");
extern int g_CdPendingReadCount __asm__("D_800A3608");

void CQ_clear_queue(void *queue);

void Spu_DrainQueueEntry(void) {
    int tag;

    tag = g_CdDsReadQueue[g_CdDsReadIndex].active;
    while (g_CdPendingReadCount > 0) {
        CQ_clear_queue(&g_CdDsReadQueue[g_CdDsReadIndex]);

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
