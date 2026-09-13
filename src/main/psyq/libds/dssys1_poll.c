/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"
extern u32 D_8009B578;
extern DsEventCallback volatile D_800A36AC;
#define STATE(p)                                                                       \
    ((CdRomCommandState *)((u8 *)(p) - PE1_OFFSETOF(CdRomCommandState, read.command)))
#define SYSTEM(p)                                                                      \
    ((CdRomSystemState *)((u8 *)(p) -                                                  \
                          PE1_OFFSETOF(CdRomSystemState, command.read.command)))
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
                            if (D_800A36AC) {
                                callbackEvent = 5;
                                callback = D_800A36AC;
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
                            register DsEventCallback available = D_800A36AC;
                            register int value asm("$2") = 1;
                            STATE(command)->read.status = value;
                            value = 11;
                            *command = value;
                            STATE(command)->read.sector = 0;
                            if (available) {
                                callbackEvent = 2;
                                callback = D_800A36AC;
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
