/* ASSEMBLER: GNU */
#include "pe1/psyq_ds_queue.h"
void Spu_DrainQueueEntry(void);
int CdRom_TryIssueCmd(int, void *);
#define QUEUE_FROM_CURRENT(p)                                                          \
    ((CdDsReadQueueWindow *)((u8 *)(p) - PE1_OFFSETOF(CdDsReadQueueWindow, read_index)))
void LIBDS_DSSYS_2_text_3D0(int inEvent, u8 *inResult) {
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
            register int offset asm("$2") = index * sizeof(CdDsReadQueueEntry);
            entry = (CdDsReadQueueEntry *)((u32)offset + (u32)base);
        }
    }
    savedEvent = event;
    if (entry->active) {
        {
            register CdQueuedCmdSlot *slot = &g_CdQueuedCmdSlots[0];
            asm("" : "=r"(slot) : "0"(slot));
            slot->state = entry->active;
            slot->result = event;
            Util_Copy8(slot->payload, result);
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
                CdRom_EnqueueCmd(entry->active, 2, result);
                {
                    register DsEventCallback cb = (DsEventCallback)entry->arg10;
                    if (cb)
                        cb(2, result);
                }
                Spu_DrainQueueEntry();
            } else {
                *current = next;
                if (!valid) {
                    *current = 0;
                }
            }
            break;
        }
        case 5: {
            int retries = (int)entry->arg14;
            if (retries > 0 || retries == -1) {
                *current = ((CdDsReadQueueWindow *)base)->queue_state;
                if ((int)entry->arg14 != -1)
                    entry->arg14--;
            } else {
                CdRom_EnqueueCmd(entry->active, 5, result);
                {
                    register DsEventCallback cb = (DsEventCallback)entry->arg10;
                    if (cb)
                        cb(5, result);
                }
                Spu_DrainQueueEntry();
            }
            break;
        }
        }
    }
    if (g_DsReadCallbackState.start)
        ((DsEventCallback)g_DsReadCallbackState.start)(savedEvent, result);
    {
        int ready = DsSync(0);
        if (ready == 1) {
            int *pending = &D_800A3608;
            if (*pending > 0 && DsSync(0) == ready) {
                register int index = D_800A3604;
                register CdDsReadQueueEntry *item;
                {
                    register int offset asm("$2") = index * sizeof(CdDsReadQueueEntry);
                    register u8 *queue;
                    queue = (u8 *)pending -
                            PE1_OFFSETOF(CdDsReadQueueWindow, pending_count);
                    item = (CdDsReadQueueEntry *)((u32)offset + (u32)queue);
                }
                if (item->active)
                    CdRom_TryIssueCmd(item->command, item->parameter);
            }
        }
    }
}
