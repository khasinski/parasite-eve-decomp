/* ASSEMBLER: GNU */
/* Psy-Q LIBDS/DSSYS_1.OBJ private text_8B8.
 * See proposals/PsyqPadDsFour for byte-match evidence and constraint debt. */
#include "pe1/psyq_cd.h"
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
extern DsReadStatusBlock D_8009B574;
void Render_DrawParticleSprite(int inEvent, u8 *inResult) {
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
            register DsReadStatusBlock *read = &D_8009B574;
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
