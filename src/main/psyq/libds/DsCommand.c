/* ASSEMBLER: GNU */
#include "pe1/psyq_ds_queue.h"

/* Empty constraints preserve the retail schedule. */

int DsCommand(int inCommand, void *inParameter, DslCB inCallback,
                             int inCount) {
    register void *parameter = inParameter;
    register DslCB callback = inCallback;
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
            entry = CQ_last_queue();
            entry->active = ticket;
            entry->command = 2;
            asm("" : "=r"(parameter) : "0"(parameter));
            if (parameter) {
                parcpy(entry->payload, parameter);
                entry->parameter = parameter;
            } else {
                entry->parameter = 0;
            }
            entry->callback = 0;
            entry->count = 0;
            asm("" ::: "memory");
            D_800A3608++;
            if (DS_system_status(0) == 1 && D_800A3540[D_800A3604].active == ticket)
                CQ_execute();
            result = ticket;
        } else
            result = 0;

        if (!result) {
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
        entry = CQ_last_queue();
        entry->active = ticket;
        entry->command = command;
        if (parameter) {
            parcpy(entry->payload, parameter);
            entry->parameter = parameter;
        } else {
            entry->parameter = 0;
        }
        entry->callback = callback;
        entry->count = count;
        asm("" ::: "memory");
        D_800A3608++;
        if (DS_system_status(0) == 1 && D_800A3540[D_800A3604].active == ticket)
            CQ_execute();
        result = ticket;
    } else
        result = 0;
    return result;
}

/* Empty constraints preserve the retail schedule. */

int DsPacket(int inMode, DslLOC *inPosition, int inCommand,
                              DslCB inCallback, int inCount) {
    DsPacketCommand commands[5];
    register CdlLOC *position = inPosition;
    register CdlLOC *callPosition asm("$4");
    register DslCB callback = inCallback;
    register int mode = inMode;
    register int command asm("$17") = inCommand;
    register unsigned count = inCount;
    register int n;
    register int i, total;
    register DsPacketCommand *ptr asm("$3"), *list, *item;
    register CdDsReadQueueEntry *entry;
    register DslCB *field asm("$18");
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
    if (DsPosToInt(callPosition) < 0) {
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
                entry = CQ_last_queue();
                asm("" : "=r"(entry) : "0"(entry));
                if (!entry) {
                    goto failure;
                }
                entry->active = ticket;
                entry->command = list->command;
                payload = entry->payload;
                if (field[-1]) {
                    parcpy(payload, &list->payload);
                    entry->parameter = payload;
                } else
                    entry->parameter = 0;
                entry->callback = *field;
                field += 4;
                entry->count = count;
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
                entry = CQ_last_queue();
                asm("" : "=r"(entry) : "0"(entry));
                if (!entry) {
                    goto failure;
                }
                entry->active = ticket;
                entry->command = list->command;
                payload = entry->payload;
                if (field[-1]) {
                    parcpy(payload, &list->payload);
                    entry->parameter = payload;
                } else
                    entry->parameter = 0;
                entry->callback = *field;
                field += 4;
                entry->count = count;
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
    if (DS_system_status(0) == 1 && D_800A3540[D_800A3604].active == ticket)
        CQ_execute();
    result = ticket;
    goto done;
failure:
    result = 0;
done:
    return result;
}

/* Empty constraints preserve the retail schedule. */

/* Word-offset views of the eight 16-byte history records. */
extern int D_800A3610[], D_800A3614[], D_800A3618[], D_800A361C[];
extern int D_800A3690;
extern DsResult D_800A3500;

s32 DsSync(s32 inId, void *inResult) {
    register s32 id = inId;
    register void *result = inResult;
    register DsResult *selected asm("$16");
    register s32 cursor;
    register s32 scanned;
    register s32 searched;
    register s32 offset;
    register s32 status;
    register s32 entryIdOrOffset;
    register s32 forward;
    register s32 readiness;
    register s32 backward;

    if (id != 0) {
        DS_sync(0);
        forward = D_800A3690;
        scanned = 0;
        entryIdOrOffset = forward * 0x10;
    scanForward:
        entryIdOrOffset = *(int *)((char *)D_800A3610 + entryIdOrOffset);
        forward += 1;
        if (entryIdOrOffset != id) {
            if (forward >= 8) {
                forward = 0;
            }
            scanned += 1;
            if (scanned >= 8) {
                readiness = 0;
                if (id < *(int *)((char *)D_800A3610 + (D_800A3690 * 0x10))) {
                    goto completed;
                }
            } else {
                entryIdOrOffset = forward * 0x10;
                goto scanForward;
            }
        } else {
        completed:
            readiness = 7;
        }
        if (readiness != 7) {
            status = 0;
            goto done;
        }
        {
            cursor = D_800A3690;
            backward = cursor - 1;
            if (backward < 0) {
                backward = 7;
            }
            searched = 0;
            if (id != 0) {
                offset = backward * 0x10;
            scanBackward:
                backward -= 1;
                if (*(int *)((char *)D_800A3610 + offset) != id) {
                    if (backward < 0) {
                        backward = 7;
                    }
                    searched += 1;
                    offset = backward * 0x10;
                    if (searched >= 8) {
                        selected = 0;
                    } else {
                        goto scanBackward;
                    }
                } else {
                    goto publishResult;
                }
            } else {
                offset = backward * 0x10;
                selected = 0;
                if (*(int *)((char *)D_800A3610 + offset) == 0) {

                } else {
                    goto publishResult;
                }
            }
            goto copyResult;
        }
    }
    cursor = D_800A3690 - 1;
    offset = cursor * 0x10;
    if (cursor < 0) {
        cursor = 7;
        asm("" : "=r"(cursor) : "0"(cursor));
        offset = cursor * 0x10;
    }
    selected = 0;
    if (*(int *)((char *)D_800A3610 + offset) != 0) {
    publishResult:
        {
            register int *dst = D_800A3500.words;
            register int first asm("$2");
            register int second asm("$3");
            register int third asm("$4");
            asm("" : "=r"(dst) : "0"(dst) : "memory");
            first = *(int *)((char *)D_800A3610 + offset);
            second = *(int *)((char *)D_800A3614 + offset);
            third = *(int *)((char *)D_800A3618 + offset);
            asm(""
                : "=r"(first), "=r"(second), "=r"(third)
                : "0"(first), "1"(second), "2"(third));
            dst[0] = first;
            dst[1] = second;
            dst[2] = third;
            dst[3] = *(int *)((char *)D_800A361C + offset);
        }
        asm("" ::: "$16", "memory");
        selected = &D_800A3500;
    }
copyResult:
    if (selected != 0) {
        rescpy(result, selected->slot.payload);
        status = selected->slot.command;
        goto done;
    }
    status = 6;
done:
    return status;
}
