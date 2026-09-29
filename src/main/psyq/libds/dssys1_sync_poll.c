/* ASSEMBLER: GNU */
#include "pe1/psyq_ds.h"

/* Contiguous private routines from Psy-Q LIBDS/DSSYS_1.OBJ:
 * text_8B8 is at 0x80080220 and text_A9C follows at 0x80080404.
 * text_4A4 is non-contiguous and remains in dssys1_vsync.c. */
/* Psy-Q LIBDS/DSSYS_1.OBJ private text_8B8.
 * See proposals/PsyqPadDsFour for byte-match evidence and constraint debt. */
#define STATE_FROM_MODE(p)                                                             \
    ((CdRomEventCommandState *)((u8 *)(p) - PE1_OFFSETOF(CdRomEventCommandState,       \
                                                         command.read.commandMode)))
#define STATE_FROM_RETRY(p)                                                            \
    ((CdRomEventCommandState *)((u8 *)(p) - PE1_OFFSETOF(CdRomEventCommandState,       \
                                                         command.read.retryCount)))
#define STATE_FROM_PARAM(p)                                                            \
    ((CdRomEventCommandState *)((u8 *)(p) - PE1_OFFSETOF(CdRomEventCommandState,       \
                                                         command.read.commandParam)))
extern u8 D_8009B581, D_8009B586, D_8009B587;
extern CdlLOC D_8009B582;

extern u32 D_8009B578;
#define STATE(p)                                                                       \
    ((CdRomCommandState *)((u8 *)(p) - PE1_OFFSETOF(CdRomCommandState, read.command)))
#define SYSTEM(p)                                                                      \
    ((CdRomSystemState *)((u8 *)(p) -                                                  \
                          CDROM_SYSTEM_READ_COMMAND_OFFSET))

void LIBDS_DSSYS_1_text_8B8(int inEvent, u8 *inResult) {
    register int event = inEvent;
    register u8 *result = inResult;
    register int masked = (u8)event;
    register int two;
    two = 2;
    if (masked == two) {
        register CdRomEventCommandState *state = &g_CdRomEventCommandState;
        register int pending;
        register int command;
        register int value;
        asm("" : "=r"(state) : "0"(state) : "$2");
        pending = state->pendingCommand;
        value = 14;
        asm("" : "=r"(pending) : "0"(pending) : "memory");
        command = (u8)pending;
        if (command == value) {
            if ((state->command.read.commandMode ^ state->pendingParamBytes[0]) & 128) {
                state->command.read.command = 15;
                state->command.read.status = two;
                state->command.read.syncResult = 3;
            } else {
                state->command.read.status = 1;
                state->command.read.command = 11;
            }
            {
                register u8 *mode asm("$3") = &D_8009B581;
                asm("" : "=r"(mode) : "0"(mode));
                *mode = STATE_FROM_MODE(mode)->pendingParamBytes[0];
            }
            goto dispatch;
        }
        if (command == 3) {
            state->command.read.command = 16;
            goto readCommand;
        } else if (command == 6 || command == 27) {
            state->command.read.command = 17;
        readCommand:
            state->command.read.status = two;
            state->command.read.commandParam = pending;
            state->command.reserved34 = 1200;
            goto dispatch;
        }
        switch (state->pendingCommand) {
        case 2:
            D_8009B582 = *(CdlLOC *)g_CdRomEventCommandState.pendingParamBytes;
            break;
        case 21:
        case 22: {
            register u8 *retry asm("$3") = &D_8009B586;
            asm("" : "=r"(retry) : "0"(retry));
            *retry = STATE_FROM_RETRY(retry)->pendingCommand;
            break;
        }
        case 3:
        case 6:
        case 27: {
            register u8 *param asm("$3") = &D_8009B587;
            asm("" : "=r"(param) : "0"(param));
            *param = STATE_FROM_PARAM(param)->pendingCommand;
            break;
        }
        }
        {
            register DsReadStatusBlock *read = &g_DsReadStatusBlock;
            asm("" : "=r"(read) : "0"(read));
            read->status = 1;
            read->command = 11;
        }
    } else {
        register CdRomCommandState *state = &g_CdSeekState;
        asm("" : "=r"(state) : "0"(state));
        if (state->eventStatus & 16) {
            state->read.status = two;
            state->read.command = 12;
        } else {
            state->read.status = 1;
            state->read.command = 11;
        }
    }
dispatch:
    if (g_DsSyncCallback && g_DsReadSysEnabled.enabled) {
        register DsEventCallback cb = g_DsSyncCallback;
        cb(event, result);
    }
}

void LIBDS_DSSYS_1_text_A9C(int event, u8 *inResult) {
    u8 *result = inResult;
    register u32 *command = &D_8009B578;
    register int twelve;
    register int old;
    register int two;
    register DsEventCallback callback;
    register int callbackEvent;
    asm("" : "=r"(command) : "0"(command));
    old = *command;
    twelve = 12;
    if (old == twelve)
        *command = 13;
    {
        register int masked = (u8)event;
        two = 2;
        if (masked == two) {
            register int current = *command;
            register u32 range;
            if (current == 13) {
                if (!(STATE(command)->eventStatus & 16)) {
                    *command = 14;
                    STATE(command)->read.status = two;
                    STATE(command)->read.sector = 21;
                    STATE(command)->read.discType++;
                }
                goto done;
            } else {
                if (current == 14) {
                    register int step = STATE(command)->read.sector;
                    register int next;
                    if (step == 21) {
                        if (!(STATE(command)->eventStatus & 16)) {
                            STATE(command)->read.status = two;
                            *command = current;
                            STATE(command)->read.sector = 22;
                            STATE(command)->read.reserved18 = 0;
                        }
                    } else if (step == 22) {
                        if (STATE(command)->eventStatus & 2) {
                            next = 23;
                            goto advance;
                        }
                        if ((int)STATE(command)->read.reserved18 >= 301) {
                            STATE(command)->read.status = 3;
                            if (g_DsDispatchCallback) {
                                callbackEvent = 5;
                                callback = g_DsDispatchCallback;
                                goto notify;
                            }
                        }
                    } else if (step == 23) {
                        next = 24;
                    advance:
                        STATE(command)->read.status = two;
                        *command = current;
                        STATE(command)->read.sector = next;
                    } else if (step == 24) {
                        if (STATE(command)->eventStatus == two) {
                            register DsEventCallback available = g_DsDispatchCallback;
                            register int value asm("$2") = 1;
                            STATE(command)->read.status = value;
                            value = 11;
                            *command = value;
                            STATE(command)->read.sector = 0;
                            if (available) {
                                callbackEvent = 2;
                                callback = g_DsDispatchCallback;
                                goto notify;
                            }
                        }
                    }
                    goto done;
                } else {
                    range = (u32)current - 16;
                    if (range < 2) {
                        if (STATE(command)->reserved34 == 0 &&
                            !(STATE(command)->eventStatus & 2)) {
                            {
                                register DsEventCallback available = g_DsReadyCallback;
                                register int value asm("$2") = 1;
                                STATE(command)->read.status = value;
                                value = 11;
                                *command = value;
                                if (available && SYSTEM(command)->enabled)
                                    g_DsReadyCallback(5, result);
                            }
                            if (g_DsSyncCallback && g_DsReadSysEnabled.enabled) {
                                callbackEvent = 5;
                                callback = g_DsSyncCallback;
                            notify:
                                callback(callbackEvent, result);
                            }
                        }
                        goto done;
                    }
                }
            }
        } else {
            if (STATE(command)->eventStatus & 16) {
                STATE(command)->read.status = two;
                *command = twelve;
            } else if ((u32)(*command - 16) < 2) {
                STATE(command)->read.status = 1;
                *command = 11;
            }
        }
    }
done:;
}
