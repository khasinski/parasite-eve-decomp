/* ASSEMBLER: GNU */
#include "pe1/psyq_ds_queue.h"
void CQ_delete_command(void);
int DS_cw(int, void *);
#define QUEUE_FROM_CURRENT(p)                                                          \
    ((CdDsReadQueueWindow *)((u8 *)(p) - PE1_OFFSETOF(CdDsReadQueueWindow, read_index)))
void CQ_sync_system(int inEvent, u8 *inResult) {
    register int event = inEvent;
    register u8 *result = inResult;
    register int *current;
    register CdDsReadQueueEntry *base asm("$19");
    register CdDsReadQueueEntry *entry;
    register int savedEvent asm("$20");
    current = &D_800A3604;
    {
        int index = *current;
        asm("" : : "r"(index));
        base = QUEUE_FROM_CURRENT(current)->entries;
        asm("" : "=r"(base) : "0"(base));
        {
            int offset = index * sizeof(CdDsReadQueueEntry);
            entry = (CdDsReadQueueEntry *)(offset + (u32)base);
        }
    }
    savedEvent = event;
    if (entry->active) {
        {
            register CdQueuedCmdSlot *slot = &g_CdQueuedCmdSlots[0];
            asm("" : "=r"(slot) : "0"(slot));
            slot->state = entry->active;
            slot->result = event;
            rescpy(slot->payload, result);
        }
        switch ((u8)savedEvent) {
        case 2: {
            register int indexValue = *current;
            register int next = indexValue + 1;
            register int valid = next < 8;
            int index = next;
            if (!valid)
                index = 0;
            if (base[index].active != entry->active) {
                CQ_add_result(entry->active, 2, result);
                {
                    register DslCB cb = entry->callback;
                    if (cb)
                        cb(2, result);
                }
                CQ_delete_command();
            } else {
                *current = next;
                if (!valid) {
                    *current = 0;
                }
            }
            break;
        }
        case 5: {
            int retries = entry->count;
            if (retries > 0 || retries == -1) {
                *current = ((CdDsReadQueueWindow *)base)->queue_state;
                if (entry->count != -1)
                    entry->count--;
            } else {
                CQ_add_result(entry->active, 5, result);
                {
                    register DslCB cb = entry->callback;
                    if (cb)
                        cb(5, result);
                }
                CQ_delete_command();
            }
            break;
        }
        }
    }
    if (g_DsReadCallbackState.start)
        g_DsReadCallbackState.start(savedEvent, result);
    {
        int ready = DS_system_status(0);
        if (ready == 1) {
            int *pending = &D_800A3608;
            if (*pending > 0 && DS_system_status(0) == ready) {
                register int index = D_800A3604;
                register CdDsReadQueueEntry *item;
                {
                    int offset = index * sizeof(CdDsReadQueueEntry);
                    register u8 *queue;
                    queue = (u8 *)pending -
                            PE1_OFFSETOF(CdDsReadQueueWindow, pending_count);
                    item = (CdDsReadQueueEntry *)(offset + queue);
                }
                if (item->active)
                    DS_cw(item->command, item->parameter);
            }
        }
    }
}
