#include "common.h"

extern u8 * volatile D_800C0DC8;
extern int D_800C20C4;
extern int D_800BE9EC;

typedef struct StRingEntry {
    s16 status;
    u8 pad_02[4];
    u16 count;
    u8 pad_08[0x18];
} StRingEntry;

int func_8007C394(u8 *ptr) {
    u8 *base;
    int index;
    int i;
    int count;
    int limit;
    StRingEntry *entry;
    int status;
    int raw_count;
    int expected_status;
    int next_index;
    base = D_800C0DC8;
    index = ((ptr - (base + (D_800C20C4 << 5))) >> 2) / 504;
    entry = (StRingEntry *)(base + (index << 5));
    /* Keep the status literal from scheduling before the entry address math. */
    
    expected_status = 4;
    status = entry->status;
    raw_count = entry->count;
    if (status != expected_status) {
        return 1;
    }

    count = (s16)raw_count;
    i = 0;
    if (count > 0) {
        limit = count;
        do {
            int slot = i + index;
            i++;
            ((StRingEntry *)(D_800C0DC8 + (slot << 5)))->status = 0;
        } while (i < limit);
    }

    next_index = i + index;
    D_800BE9EC = next_index;
    return 0;
}
