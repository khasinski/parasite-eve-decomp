#include "common.h"
#include "pe1/psyq_cd.h"

/* GCC_VERSION: 2.8.1 */

extern CdRomSystemState D_8009B554;
extern DsReadStatusBlock g_DsReadStatusBlock __asm__("D_8009B574");

void CD_flush(void);

void CdRom_AbortCmd(void)
{
    CdRomSystemState *state;
    u32 kind;
    u32 cmp;

    state = &D_8009B554;
    __asm__ volatile("" : "=r"(state) : "0"(state));
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
            slot = &g_DsReadStatusBlock;
            __asm__ volatile("" : "=r"(slot) : "0"(slot));
            value = 1;
            slot->status = value;
            value = 0xB;
            slot->command = value;
        }
    }
}
