/* GCC_VERSION: 2.8.1 */
/* ASSEMBLER: GNU */
/* CC1_FLAGS: -mno-split-addresses -fno-thread-jumps */
#include "pe1/psyq_ds.h"

void Spu_DrainQueueEntry(void) {
    int *state;
    int *initial;
    int *end;
    int *count;
    CdDsReadQueueEntry *entries;
    CdDsReadQueueEntry *entry;
    unsigned int active;
    int i;
    int index;
    unsigned char *ptr;

    initial = &g_CdDsReadQueueState;
    asm volatile("" : "=r"(initial) : "0"(initial));
    count = initial + 2;
    entries = (CdDsReadQueueEntry *)((char *)initial -
        PE1_OFFSETOF(CdDsReadQueueWindow, queue_state));
    {
        int index = initial[0];
        state = initial;
        active = g_CdDsReadQueue[index].active;
    }
again:
    if (*count <= 0) goto done;
    index = g_CdDsReadQueueState;
    /* Keep the counter setup after the queue index load. */
    asm volatile("" : : "r"(index) : "$4");
    i = 3;
    /* Do not reuse the counter constant in address arithmetic. */
    asm volatile("" : "=r"(i) : "0"(i), "r"(index));
    entry = (CdDsReadQueueEntry *)
        ((index * sizeof(CdDsReadQueueEntry)) + (int)entries);
    entry->active = 0;
    ptr = (unsigned char *)entry + 3;
    entry->command = 0;
    do {
        ptr[5] = 0;
        i--;
        ptr--;
    } while (i >= 0);
    entry->parameter = 0;
    entry->callback = 0;
    entry->count = 0;
    state[0]++;
    if (state[0] >= 8) state[0] = 0;
    state[2]--;
    if (g_CdDsReadQueue[state[0]].active == active) goto again;
done:
    end = &g_CdDsReadIndex;
    *end = end[-1];
}
