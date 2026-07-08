#include "pe1/psyq_cd.h"

extern int g_CdPendingReadCount;

CdDsReadQueueEntry *Spu_GetQueueEntryPtr(void) {
    int *base;
    int index;
    int delta;
    int offset;
    int scaled;
    CdDsReadQueueEntry *entry_base;

    base = &g_CdPendingReadCount;
    asm volatile("" : "=r"(base) : "0"(base));
    index = base[0];
    if (index >= 8) {
        return 0;
    }

    delta = base[-2];
    asm volatile("addu %0,%1,%0" : "=r"(index) : "r"(delta), "0"(index));
    if (index >= 8) {
        index -= 8;
    }

    offset = index * 3;
    scaled = offset << 3;
    entry_base = (CdDsReadQueueEntry *)(base - 50);
    asm volatile("addu %0,%0,%1" : "=r"(scaled) : "r"(entry_base), "0"(scaled));
    return (CdDsReadQueueEntry *)scaled;
}
