/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Retail keeps one base address for each initialization block. */

#include "common.h"
#include "pe1/psyq_cd.h"

extern u_int D_8009B560[3];

CdlLOC *CdIntToPos(int sector, CdlLOC *position);

void CdRom_InitCmdState(void) {
    CdRomSystemState *state;
    u_char *preSeek;
    CdRomCommandState *command;
    u_char *cursor;
    int i;
    i = 3;
    state = &g_DsReadSysEnabled;
    /* Clear pendingMode and the first three reserved bytes backwards. */
    cursor = (u_char *)state + 8;
    state->enabled = 0;
    state->pendingCommand = 0;
    do {
        *cursor = 0;
        i--;
        cursor--;
    } while (i >= 0);

    preSeek = (u_char *)&D_8009B560;
    *(u_int *)preSeek = 0;
    i = 7;
    preSeek += 11;
    do {
        *preSeek = 0;
        i--;
        preSeek--;
    } while (i >= 0);

    command = &g_CdSeekState;
    command->read.status = 2;
    command->read.command = 14;
    command->eventStatus = 0;
    command->eventValue = 0;
    command->read.sector = 21;
    command->read.lastCommand = 0;
    command->read.commandMode = 0;
    CdIntToPos(0, &command->read.currentPos);
    command->read.retryCount = 0;
    command->read.commandParam = 0;
    command->read.eventFlags.bit7 = 0;
    command->read.eventFlags.bit6 = 0;
    command->read.eventFlags.bit5 = 0;
    command->read.eventFlags.bit1 = 0;
    command->read.reserved18 = 0;
    command->read.discType = 1;
    command->read.syncResult = 0;
    command->retryAttempts = 0;
    command->read.readyResult = 0;
    command->reserved34 = 0;
}
