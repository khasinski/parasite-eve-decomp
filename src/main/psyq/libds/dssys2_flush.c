/* ASSEMBLER: GNU */
/* Psy-Q LIBDS/DSSYS_2.OBJ private text_170.
 * See proposals/PsyqPadDsFour for byte-match evidence and constraint debt. */
#include "pe1/psyq_ds_queue.h"
void LIBDS_DSSYS_2_text_170(int event, u8 *data) {
    DsQueueCallback callbacks[8];
    register u8 *result = data;
    register int savedEvent asm("$22") = event;
    register int i;
    register int last;
    register int index;

    register u32 id;
    u32 previous;
    {
        DsQueueCallback *init;
        i = 0;
        init = callbacks;
        for (; i < 8; i++, init++) {
            init->id = 0;
            init->callback = 0;
            asm("" : : "m"(*init));
        }
    }
    {
        register DsQueueIndices *indices asm("$3") = &D_800A3600;
        asm("" : "=r"(indices) : "0"(indices));
        previous = 0;
        last = -1;
        index = indices->start;
        i = 0;
        if (indices->count > 0)
            do {
                DsEventCallback callback;
                id = D_800A3540[index].active;
                if (id != previous) {
                    CdRom_EnqueueCmd(id, savedEvent, result);
                    previous = id;
                }
                callback = (DsEventCallback)D_800A3540[index].arg10;
                if (callback) {
                    {
                        register int sentinel asm("$7") = -1;
                        if (last == sentinel) {
                            last = 0;
                            callbacks[0].id = id;
                            goto saveCallback;
                        } else if (callbacks[last].id != id) {
                            last++;
                            callbacks[last].id = id;
                            goto saveCallback;
                        }
                        goto skipCallback;
                    saveCallback:
                        callbacks[last].callback = callback;
                    skipCallback:;
                    }
                }
                index++;
                if (index >= 8)
                    index = 0;
                i++;
            } while (i < D_800A3608);
    }
    D_800A3608 = 0;
    D_800A3604 = 0;
    D_800A3600.start = 0;
    {
        register int n;
        register int j;
        register CdDsReadQueueEntry *entry;
        n = 0;
        entry = D_800A3540;
        for (; n < 8; n++, entry++) {
            asm("" : "=r"(entry) : "0"(entry));
            entry->active = 0;
            entry->command = 0;
            {
                register u8 *clear;
                j = 3;
                asm("" : : "r"(j));
                /* Reverse byte traversal retains the queue-entry base. */
                clear = (u8 *)entry + 3;
                for (; j >= 0; j--, clear--)
                    clear[PE1_OFFSETOF(CdDsReadQueueEntry, payload)] = 0;
            }
            entry->parameter = 0;
            entry->arg10 = 0;
            entry->arg14 = 0;
            asm("" : : "m"(*entry));
        }
    }
    {
        i = 0;
        if (last >= 0) {
            DsQueueCallback *call = callbacks;
            do {
                register int eventArg asm("$4") = (u8)savedEvent;
                register u8 *dataArg asm("$5") = result;
                register DsEventCallback cb;
                asm("" : : "r"(eventArg), "r"(dataArg));
                cb = call->callback;
                call++;
                i++;
                cb(eventArg, dataArg);
            } while (i <= last);
        }
    }
}
