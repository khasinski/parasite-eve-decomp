#include "common.h"
#include "pe1/psyq_cd.h"

/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

extern CdRomSystemState D_8009B554;
extern DsReadStatusBlock D_8009B574;

void CD_flush(void);

void CdRom_AbortCmd(void)
{
    CdRomSystemState *state;
    u32 kind;
    u32 cmp;

    state = &D_8009B554;
    state->enabled = 0;
    CD_flush();

    if (state->command.read.status == 2) {
        kind = state->command.read.command;
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
            slot = &D_8009B574;
            value = 1;
            slot->status = value;
            slot->command = 0xB;
        }
    }
}
