#include "pe1/psyq_cd.h"

extern int g_CdPendingReadCount;

CdDsReadQueueEntry *Spu_GetQueueEntryPtr(void) {
    int *base;
    int index;
    int delta;
    CdDsReadQueueEntry *entry_base;

    base = &g_CdPendingReadCount;
    index = base[0];
    if (index >= 8) {
        return 0;
    }

    delta = base[-2];
    index += delta;
    if (index >= 8) {
        index -= 8;
    }

    entry_base = (CdDsReadQueueEntry *)(base - 50);
    return entry_base + index;
}
