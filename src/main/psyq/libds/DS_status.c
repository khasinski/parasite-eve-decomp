/* GCC_VERSION: 2.8.1 */
/* ASSEMBLER: GNU */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_ds.h"

extern char D_80011D74[];
extern char D_80011D94[];
extern char D_80011DB0[];
extern char D_80011DD8[];
extern char D_80011DF8[];
extern char D_80011E0C[];
extern char D_80011E3C[];
extern char D_80011E44[];
extern char D_80011E4C[];
int printf(char *fmt, ...);

void DS_status(void) {
    enum {
        PARAM_OFFSET = PE1_OFFSETOF(CdRomEventCommandState, pendingParamBytes) -
                       PE1_OFFSETOF(CdRomEventCommandState, command.read)
    };
    DsReadStatusBlock *state;
    CdRomEventCommandState *command;
    int b0;
    int b1;
    int b2;
    int b3;
    int b4;
    char *busy;

    printf(D_80011D74);

    state = &g_DsReadStatusBlock;
    printf(D_80011D94, state->status, state->command, state->sector);
    command = (CdRomEventCommandState *)((char *)state -
        PE1_OFFSETOF(CdRomEventCommandState, command.read));
    b0 = command->pendingCommand;
    b1 = ((u_char *)state)[PARAM_OFFSET + 0];
    b2 = ((u_char *)state)[PARAM_OFFSET + 1];
    b3 = ((u_char *)state)[PARAM_OFFSET + 2];
    b4 = ((u_char *)state)[PARAM_OFFSET + 3];
    printf(D_80011DB0, b0, b1, b2, b3, b4);
    printf(D_80011DD8, state->readyResult);
    printf(D_80011DF8, state->syncResult);
    {
        DsCallbackValue poll, sync, ready;
        poll.poll = g_DsPollCallback;
        sync.event = g_DsSyncCallback;
        ready.event = g_DsReadyCallback;
        printf(D_80011E0C, poll.word, sync.word, ready.word);
    }

    {
        int active = DsRead_IsBusy();
        busy = D_80011E44;
        if (active != 0) busy = D_80011E3C;
    }
    printf(D_80011E4C, busy);
}
