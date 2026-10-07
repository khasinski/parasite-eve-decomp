/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSSYS_2.OBJ, part 5 of 5: DsClose, DsCommand, DsReady, DsFlush, DsSystemStatus, DsQueueLen, DsStatus, DsShellOpen, DsLastCom, CQ_vsync_system, CQ_ready_system, LIBDS_DSSYS_2_text_13CC and the object's zero tail. */
#include "pe1/psyq_ds_queue.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"
#include "pe1/cdrom.h"

void DS_close(void);

void DsClose(void) {
    DS_close();
}

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

extern CdQueuedCmdSlot D_800A3520;
extern CdQueuedCmdSlot D_800A3530;


int DsReady(u8 *destination) {
    CdQueuedCmdSlot *slot;
    u8 *copyDestination;
    int selector;

    slot = (CdQueuedCmdSlot *)destination;
    DS_ready(0);
    if (D_800A3530.state == 1) {
        selector = 4;
    } else {
        selector = D_800A3520.state == 1;
    }

    if (selector == 4) {
        copyDestination = (u8 *)slot;
        slot = &D_800A3530;
    } else if (selector == 1) {
        copyDestination = (u8 *)slot;
        slot = &D_800A3520;
    } else {
        return 0;
    }

    slot->state = 0;
    rescpy(copyDestination, slot->payload);
    return slot->result;
}

extern void DS_stop(void);

extern int g_CdDsReadQueueState;
extern int g_CdDsReadIndex;
extern int g_CdPendingReadCount;
extern CdDsReadQueueEntry g_CdDsReadQueue[];

void DsFlush(void) {
    int i;
    register CdDsReadQueueEntry *p asm("$4");
    int j;
    unsigned char *q;

    DS_stop();

    i = 0;
    g_CdPendingReadCount = 0;
    g_CdDsReadIndex = 0;
    g_CdDsReadQueueState = 0;
    p = g_CdDsReadQueue;

    while (i < 8) {
        j = 3;
        q = (unsigned char *)p + 3;
        p->active = 0;
        p->command = 0;
        for (; j >= 0; j--, q--) {
            q[5] = 0;
        }
        p->parameter = 0;
        p->callback = 0;
        p->count = 0;
        p++;
        i++;
    }

    DsEndReadySystem();
    DS_restart();
}

int DS_system_status(int arg0);

int DsSystemStatus(void) {
    int status = DS_system_status(0);

    if (status == 1 && DsQueueLen() > 0) {
        status = 2;
    }

    return status;
}

int DS_status(void);

int DS_shell_open(void);

int DsQueueLen(void) {
    return g_CdPendingReadCount;
}

int DsStatus(void) {
    return DS_status() & 0xFF;
}

int DsShellOpen(void) {
    return DS_shell_open();
}

int DsLastCom(void) {
    return DS_lastcom() & 0xFF;
}

extern s32 D_800A3604;

int DS_system_status(int mode);

void CQ_vsync_system(void) {
    int status;
    s32 *pending;
    s32 *workAddress;
    s32 work;
    unsigned int result;

    status = DS_system_status(0);
    if (status == 1) {
        pending = &g_CdPendingReadCount;
        if (*pending > 0) {
            result = DS_system_status(0);
            if (result == status) {
                workAddress = &D_800A3604;
                work = *workAddress;
                result = work * 3;
                result <<= 3;
                work = (s32)CD_DS_QUEUE_FROM_PENDING(pending);
                work = result + work;
                if (((CdDsReadQueueEntry *)work)->active != 0) {
                    DS_cw(((CdDsReadQueueEntry *)work)->command,
                                      ((CdDsReadQueueEntry *)work)->parameter);
                }
            }
        }
    }
}

extern int D_800B8AB4;

void CQ_error_flush(int command);

int CQ_ready_system(int command, u8 *payload) {
    register int result asm("$2");
    register CdQueuedCmdSlot *slot;
    u8 *copy_destination;
    u8 command_byte;

    command_byte = command;
    if (command_byte == 5 && (payload[0] & 0x10)) {
        CQ_error_flush(5);
    }

    switch (command_byte) {
    case 1:
    case 5:
                slot = &D_800A3520;
                copy_destination = slot->payload;
        break;
    case 4:
        slot = &D_800A3530;
        copy_destination = slot->payload;
        break;
    default:
        goto callback;
    }

    slot->state = 1;
    slot->result = command_byte;
    rescpy(copy_destination, payload);

callback:
        result = D_800B8AB4;
    if (result != 0) {
        result = ((int (*)(u8, u8 *))result)(command_byte, payload);
    }
    return result;
}

extern void (*g_DsStartCallback)(int);

void LIBDS_DSSYS_2_text_13CC(int arg0) {
    if (g_DsStartCallback != 0) {
        g_DsStartCallback((unsigned char)arg0);
    }
}

unsigned int LIBDS_DSSYS_2_pad[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};
