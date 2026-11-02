/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSSYS_2.OBJ, part 3 of 3: DsClose, DsCommand, DsReady, DsFlush, DsSystemStatus, DsQueueLen, DsStatus, DsShellOpen, DsLastCom, CQ_vsync_system, CQ_ready_system, LIBDS_DSSYS_2_text_13CC and the object's zero tail. */

#include "pe1/cdrom.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"
#include "pe1/psyq_ds_queue.h"

#include "dssys_2_3_externs.h"
#include "dssys_2_3_externs_2.h"
#include "dssys_2_3_externs_3.h"
#include "dssys_2_3_externs_5.h"

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
    if (g_DsCommandHasParam[command & 255] && parameter) {

        if (g_CdPendingReadCount < 8) {
            previous = g_DsCommandSerial;
            serial = previous + 1;
            g_DsCommandSerial = serial;
            if (!serial) {
                serial = previous + 2;
                g_DsCommandSerial = serial;
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
            g_CdPendingReadCount++;
            if (DS_system_status(0) == 1 && g_CdDsReadQueue[g_CdDsReadIndex].active == ticket)
                CQ_execute();
            result = ticket;
        } else
            result = 0;

        if (!result) {
            return 0;
        }
    }

    if (g_CdPendingReadCount < 8) {
        previous = g_DsCommandSerial;
        serial = previous + 1;
        g_DsCommandSerial = serial;
        if (!serial) {
            serial = previous + 2;
            g_DsCommandSerial = serial;
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
        g_CdPendingReadCount++;
        if (DS_system_status(0) == 1 && g_CdDsReadQueue[g_CdDsReadIndex].active == ticket)
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
        result = 0; return result;
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
        if (g_CdPendingReadCount + total >= 9) {
            result = 0; return result;
        }
        previous = g_DsCommandSerial;
        next = previous + 1;
        g_DsCommandSerial = next;
        ticket = next;
        if (!next) {
            next = previous + 2;
            g_DsCommandSerial = next;
            ticket = next;
        }
        i = 0;
        if (total > 0) {
            field = &list->callback;
            do {
                entry = CQ_last_queue();
                asm("" : "=r"(entry) : "0"(entry));
                if (!entry) {
                    result = 0; return result;
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
                next = g_CdPendingReadCount;
                asm("" : "=r"(next), "=r"(i) : "0"(next), "1"(i));
                i++;
                g_CdPendingReadCount = next + 1;
                list++;
            } while (i < total);
        }
        break;
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
        if (g_CdPendingReadCount + total >= 9) {
            result = 0; return result;
        }
        previous = g_DsCommandSerial;
        next = previous + 1;
        g_DsCommandSerial = next;
        ticket = next;
        if (!next) {
            next = previous + 2;
            g_DsCommandSerial = next;
            ticket = next;
        }
        i = 0;
        if (total > 0) {
            field = &list->callback;
            do {
                entry = CQ_last_queue();
                asm("" : "=r"(entry) : "0"(entry));
                if (!entry) {
                    result = 0; return result;
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
                next = g_CdPendingReadCount;
                asm("" : "=r"(next), "=r"(i) : "0"(next), "1"(i));
                i++;
                g_CdPendingReadCount = next + 1;
                list++;
            } while (i < total);
        }
        break;
    default:
        goto failure;
    }
dispatch:
    if (DS_system_status(0) == 1 && g_CdDsReadQueue[g_CdDsReadIndex].active == ticket)
        CQ_execute();
    result = ticket;
    return result;
failure:
    result = 0;
done:
    return result;
}

/* Empty constraints preserve the retail schedule. */

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
        forward = g_DsReadCallbackCursor;
        scanned = 0;
        entryIdOrOffset = forward * 0x10;
    scanForward:
        {
            DsSlotAddress at;
            at.slot = g_DsReadCallbackSlots;
            at.word += entryIdOrOffset;
            entryIdOrOffset = *at.words;
        }
        forward += 1;
        if (entryIdOrOffset != id) {
            if (forward >= 8) {
                forward = 0;
            }
            scanned += 1;
            if (scanned >= 8) {
                readiness = 0;
                {
                    DsSlotAddress at;
                    at.slot = g_DsReadCallbackSlots;
                    at.word += g_DsReadCallbackCursor * 0x10;
                    if (id < *at.words) {
                        goto completed;
                    }
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
            return status;
        }
        {
            cursor = g_DsReadCallbackCursor;
            backward = cursor - 1;
            if (backward < 0) {
                backward = 7;
            }
            searched = 0;
            if (id != 0) {
                offset = backward * 0x10;
                while (1) {
                    DsSlotAddress at;
                    backward -= 1;
                    at.slot = g_DsReadCallbackSlots;
                    at.word += offset;
                    if (*at.words != id) {
                        if (backward < 0) {
                            backward = 7;
                        }
                        searched += 1;
                        offset = backward * 0x10;
                        if (searched >= 8) {
                            selected = 0;
                        } else {
                            continue;
                        }
                    } else {
                        goto publishResult;
                    }
                    break;
                }
            } else {
                DsSlotAddress at;
                offset = backward * 0x10;
                selected = 0;
                at.slot = g_DsReadCallbackSlots;
                at.word += offset;
                if (*at.words == 0) {

                } else {
                    goto publishResult;
                }
            }
            goto copyResult;
        }
    }
    cursor = g_DsReadCallbackCursor - 1;
    offset = cursor * 0x10;
    if (cursor < 0) {
        cursor = 7;
        asm("" : "=r"(cursor) : "0"(cursor));
        offset = cursor * 0x10;
    }
    selected = 0;
    {
    DsSlotAddress at;
    at.slot = g_DsReadCallbackSlots;
    at.word += offset;
    if (*at.words != 0) {
    publishResult:
        {
            register int *dst = g_DsSlotResultCopy.words;
            register int first asm("$2");
            register int second asm("$3");
            register int third asm("$4");
            asm("" : "=r"(dst) : "0"(dst) : "memory");
            {
                DsSlotAddress word0, word1, word2;
                word0.slot = g_DsReadCallbackSlots;
                word0.word += offset;
                first = *word0.words;
                word1.words = (int *)g_DsReadCallbackSlots + 1;
                word1.word += offset;
                second = *word1.words;
                word2.words = (int *)g_DsReadCallbackSlots + 2;
                word2.word += offset;
                third = *word2.words;
            }
            asm(""
                : "=r"(first), "=r"(second), "=r"(third)
                : "0"(first), "1"(second), "2"(third));
            dst[0] = first;
            dst[1] = second;
            dst[2] = third;
            {
                DsSlotAddress word3;
                word3.words = (int *)g_DsReadCallbackSlots + 3;
                word3.word += offset;
                dst[3] = *word3.words;
            }
        }
        asm("" ::: "$16", "memory");
        selected = &g_DsSlotResultCopy;
    }
    }
copyResult:
    if (selected != 0) {
        rescpy(result, selected->slot.payload);
        status = selected->slot.command;
    } else {
        status = 6;
    }
done:
    return status;
}

int DsReady(u8 *destination) {
    CdQueuedCmdSlot *slot;
    u8 *copyDestination;
    int selector;

    slot = (CdQueuedCmdSlot *)destination;
    DS_ready(0);
    if (g_CdQueuedCmdSlots[2].state == 1) {
        selector = 4;
    } else {
        selector = g_CdQueuedCmdSlots[1].state == 1;
    }

    if (selector == 4) {
        copyDestination = (u8 *)slot;
        slot = &g_CdQueuedCmdSlots[2];
    } else if (selector == 1) {
        copyDestination = (u8 *)slot;
        slot = &g_CdQueuedCmdSlots[1];
    } else {
        return 0;
    }

    slot->state = 0;
    rescpy(copyDestination, slot->payload);
    return slot->result;
}

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

void CQ_vsync_system(void) {
    int status;
    union {
        s32 *count;
        s32 address;
    } pending;
    s32 *workAddress;
    s32 work;
    unsigned int result;

    status = DS_system_status(0);
    if (status == 1) {
        pending.count = &g_CdPendingReadCount;
        if (*pending.count > 0) {
            result = DS_system_status(0);
            if (result == status) {
                workAddress = &g_CdDsReadIndex;
                work = *workAddress;
                result = work * 3;
                result <<= 3;
                work = pending.address - PE1_OFFSETOF(CdDsReadQueueWindow, pending_count);
                work = result + work;
                {
                    CdDsQueueEntryAddress at;
                    at.word = work;
                    if (at.entry->active != 0) {
                        DS_cw(at.entry->command,
                                          at.entry->parameter);
                    }
                }
            }
        }
    }
}

void CQ_error_flush(int command);

int CQ_ready_system(int command, u8 *payload) {
    register int (*callbackFn)(u8, u8 *) asm("$2");
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
                slot = &g_CdQueuedCmdSlots[1];
                copy_destination = slot->payload;
        break;
    case 4:
        slot = &g_CdQueuedCmdSlots[2];
        copy_destination = slot->payload;
        break;
    default:
        callbackFn = (int (*)(u8, u8 *))g_DsReadCallbackState.ready;
        if (callbackFn != 0)
            return callbackFn(command_byte, payload);
        return 0;
    }

    slot->state = 1;
    slot->result = command_byte;
    rescpy(copy_destination, payload);

callback:
    callbackFn = (int (*)(u8, u8 *))g_DsReadCallbackState.ready;
    if (callbackFn != 0)
        return callbackFn(command_byte, payload);
    return 0;
}

void LIBDS_DSSYS_2_text_13CC(int arg0) {
    if (g_DsStartCallback != 0) {
        g_DsStartCallback((unsigned char)arg0);
    }
}

unsigned int LIBDS_DSSYS_2_pad[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};
