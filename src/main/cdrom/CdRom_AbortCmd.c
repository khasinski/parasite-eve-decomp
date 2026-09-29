#include "common.h"
#include "pe1/psyq_cd.h"

/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */


void CD_flush(void);

void CdRom_AbortCmd(void)
{
    CdRomSystemState *state;
    u32 kind;
    u32 cmp;

    state = &g_DsReadSysEnabled;
    state->enabled = 0;
    CD_flush();

    if (state->view.system.command.read.status == 2) {
        kind = state->view.system.command.read.command;
        cmp = 0xB;
        if (kind == cmp) {
            goto abortPending;
        }
        cmp = 0x11;
        if (kind == cmp) {
            goto abortPending;
        }
        cmp = 0x10;
        if (kind == cmp) {
            DsReadStatusBlock *slot;
            u32 value;

abortPending:
            slot = &g_DsReadStatusBlock;
            value = 1;
            slot->status = value;
            slot->command = 0xB;
        }
    }
}
