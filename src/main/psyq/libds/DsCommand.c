/* ASSEMBLER: GNU */
/* Psy-Q DsCommand. Retains the existing link symbol for callers.
 * Empty constraints preserve the retail schedule; see proposals/PsyqQueueFour. */
#include "pe1/psyq_ds_queue.h"

int Render_AllocParticleNode(int inCommand, void *inParameter, int inCallback,
                             int inCount) {
    register void *parameter = inParameter;
    register int callback = inCallback;
    register int command = inCommand;
    register int count = inCount;
    unsigned previous;
    register unsigned serial;
    register unsigned result;
    register unsigned ticket;
    register CdDsReadQueueEntry *entry;
    asm(""
        : "=r"(parameter), "=r"(callback), "=r"(command)
        : "0"(parameter), "1"(callback), "2"(command));
    if (D_8009B4BC[command & 255] && parameter) {

        if (D_800A3608 < 8) {
            previous = D_8009B53C;
            serial = previous + 1;
            D_8009B53C = serial;
            if (!serial) {
                serial = previous + 2;
                D_8009B53C = serial;
            }
            ticket = serial;
            entry = Spu_GetQueueEntryPtr();
            entry->active = ticket;
            entry->command = 2;
            asm("" : "=r"(parameter) : "0"(parameter));
            if (parameter) {
                Util_Copy4(entry->payload, parameter);
                asm("" ::: "memory");
                entry->parameter = parameter;
            } else {
                asm("" : "=r"(parameter) : "0"(parameter));
                entry->parameter = 0;
            }
            entry->arg10 = 0;
            entry->arg14 = 0;
            asm("" ::: "memory");
            D_800A3608++;
            if (DsSync(0) == 1 && D_800A3540[D_800A3604].active == ticket)
                CdRom_DispatchPendingCmd();
            result = ticket;
        } else
            result = 0;

        if (!result) {
            asm("" : "=r"(result) : "0"(result));
            return 0;
        }
    }

    if (D_800A3608 < 8) {
        previous = D_8009B53C;
        serial = previous + 1;
        D_8009B53C = serial;
        if (!serial) {
            serial = previous + 2;
            D_8009B53C = serial;
        }
        ticket = serial;
        entry = Spu_GetQueueEntryPtr();
        entry->active = ticket;
        entry->command = command;
        asm("" : "=r"(parameter) : "0"(parameter));
        if (parameter) {
            Util_Copy4(entry->payload, parameter);
            asm("" ::: "memory");
            entry->parameter = parameter;
        } else {
            asm("" : "=r"(parameter) : "0"(parameter));
            entry->parameter = 0;
        }
        entry->arg10 = callback;
        entry->arg14 = count;
        asm("" ::: "memory");
        D_800A3608++;
        if (DsSync(0) == 1 && D_800A3540[D_800A3604].active == ticket)
            CdRom_DispatchPendingCmd();
        result = ticket;
    } else
        result = 0;
    return result;
}
