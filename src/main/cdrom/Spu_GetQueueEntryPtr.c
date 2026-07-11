#include "pe1/psyq_cd.h"

extern int D_800A3608;

CdDsReadQueueEntry *Spu_GetQueueEntryPtr(void) {
    register volatile int *base asm("$3");
    int count;
    int index;
    int delta;
    int scaled;
    int byte_offset;
    unsigned char *entry_base;

    base = &D_800A3608;
    asm volatile("" : "=r"(base) : "0"(base));
    count = base[0];
    if (count >= 8) {
        return 0;
    }

    delta = base[-2];
    index = delta + count;
    if (index >= 8) {
        index -= 8;
    }

    scaled = (index << 1) + index;
    byte_offset = scaled << 3;
    entry_base = (unsigned char *)((int *)base - 50);
    return (CdDsReadQueueEntry *)(byte_offset + (int)entry_base);
}
