#include "pe1/psyq_cd.h"

CdDsReadQueueEntry *Spu_GetQueueEntryPtr(void) {
    volatile int *base;
    int count;
    int index;
    int delta;
    int scaled;
    int byte_offset;
    unsigned char *entry_base;

    base = &g_CdPendingReadCount;
    asm volatile("" : "=r"(base) : "0"(base));
    count = base[0];
    if (count >= 8) {
        return 0;
    }

    delta = ((volatile CdDsReadQueueWindow *)CD_DS_QUEUE_FROM_PENDING(base))->queue_state;
    index = delta + count;
    if (index >= 8) {
        index -= 8;
    }

    scaled = (index << 1) + index;
    byte_offset = scaled << 3;
    entry_base = (unsigned char *)CD_DS_QUEUE_FROM_PENDING(base)->entries;
    return (CdDsReadQueueEntry *)(byte_offset + (int)entry_base);
}
