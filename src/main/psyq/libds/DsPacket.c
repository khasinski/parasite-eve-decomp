/* ASSEMBLER: GNU */
/* Psy-Q DsPacket. Retains the existing link symbol for callers.
 * Empty constraints preserve the retail schedule; see proposals/PsyqQueueFour. */
#include "pe1/psyq_ds_queue.h"

int Render_BuildParticleFrame(int inMode, CdlLOC *inPosition, int inCommand,
                              int inCallback, unsigned inCount) {
    DsPacketCommand commands[5];
    register CdlLOC *position = inPosition;
    register CdlLOC *callPosition asm("$4");
    register int callback = inCallback;
    register int mode = inMode;
    register int command asm("$17") = inCommand;
    register unsigned count = inCount;
    register int n;
    register int i, total;
    register DsPacketCommand *ptr asm("$3"), *list, *item;
    register CdDsReadQueueEntry *entry;
    register unsigned *field asm("$18");
    register unsigned previous, next;
    register unsigned ticket asm("$21");
    register unsigned result;
    register unsigned char *payload;
    n = 0;
    ptr = commands;
    do {
        ptr->command = 0;
        ptr->parameter = 0;
        ptr->callback = 0;
        asm("" : "=r"(n) : "0"(n) : "memory");
        n++;
        ptr++;
    } while (n < 5);
    n = 2;
    callPosition = position;
    commands[0].command = 9;
    asm("" ::: "$3", "memory");
    next = 14;
    ptr = &commands[1];
    asm("" : "=r"(ptr) : "0"(ptr));
    commands[1].command = next;
    ptr->payload.minute = mode;
    ptr->parameter = &commands[1].payload;
    if (CdPosToInt(callPosition) < 0) {
        goto failure;
    }
    commands[2].command = n;
    commands[2].payload = *position;
    commands[2].parameter = &commands[2].payload;
    n = 3;
    switch (command & 255) {
    case 21:
    case 22:

        item = commands;
        asm("" : "=r"(item) : "0"(item));
        list = item;
        item = &list[n];
        asm("" : "=r"(item) : "0"(item));
        item->command = command;
        item->callback = callback;
        asm("" ::: "memory");
        total = n + 1;
        if (D_800A3608 + total >= 9) {
            goto failure;
        }
        previous = D_8009B53C;
        next = previous + 1;
        D_8009B53C = next;
        ticket = next;
        if (!next) {
            next = previous + 2;
            D_8009B53C = next;
            ticket = next;
        }
        i = 0;
        if (total > 0) {
            field = &list->callback;
            do {
                entry = Spu_GetQueueEntryPtr();
                asm("" : "=r"(entry) : "0"(entry));
                if (!entry) {
                    goto failure;
                }
                entry->active = ticket;
                entry->command = list->command;
                payload = entry->payload;
                if (field[-1]) {
                    Util_Copy4(payload, &list->payload);
                    entry->parameter = payload;
                } else
                    entry->parameter = 0;
                next = *field;
                field += 4;
                entry->arg14 = count;
                entry->arg10 = next;
                asm("" : "=r"(i), "=r"(list) : "0"(i), "1"(list) : "memory");
                next = D_800A3608;
                asm("" : "=r"(next), "=r"(i) : "0"(next), "1"(i));
                i++;
                D_800A3608 = next + 1;
                list++;
            } while (i < total);
        }
        goto dispatch;
    case 3:
    case 6:
    case 27:

        item = commands;
        asm("" : "=r"(item) : "0"(item));
        list = item;
        item = &list[n];
        asm("" : "=r"(item) : "0"(item));
        item->command = command;
        item->callback = callback;
        asm("" ::: "memory");
        total = n + 1;
        if (D_800A3608 + total >= 9) {
            goto failure;
        }
        previous = D_8009B53C;
        next = previous + 1;
        D_8009B53C = next;
        ticket = next;
        if (!next) {
            next = previous + 2;
            D_8009B53C = next;
            ticket = next;
        }
        i = 0;
        if (total > 0) {
            field = &list->callback;
            do {
                entry = Spu_GetQueueEntryPtr();
                asm("" : "=r"(entry) : "0"(entry));
                if (!entry) {
                    goto failure;
                }
                entry->active = ticket;
                entry->command = list->command;
                payload = entry->payload;
                if (field[-1]) {
                    Util_Copy4(payload, &list->payload);
                    entry->parameter = payload;
                } else
                    entry->parameter = 0;
                next = *field;
                field += 4;
                entry->arg14 = count;
                entry->arg10 = next;
                asm("" : "=r"(i), "=r"(list) : "0"(i), "1"(list) : "memory");
                next = D_800A3608;
                asm("" : "=r"(next), "=r"(i) : "0"(next), "1"(i));
                i++;
                D_800A3608 = next + 1;
                list++;
            } while (i < total);
        }
        goto dispatch;
    default:
        goto failure;
    }
dispatch:
    if (DsSync(0) == 1 && D_800A3540[D_800A3604].active == ticket)
        CdRom_DispatchPendingCmd();
    result = ticket;
    goto done;
failure:
    result = 0;
done:
    return result;
}
