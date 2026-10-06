/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBDS DSSYS_1.OBJ, part 9 of 11: DS_stop, DS_restart, DS_system_active. */
#include "common.h"
#include "pe1/psyq_cd.h"
#include "pe1/cdrom.h"

void CD_flush(void);

void DS_stop(void)
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

void DS_restart(void) {
    int enabled;
    enabled = 1;
    g_DsReadSysEnabled.enabled = enabled;
}

int DS_system_active(void) {
    int scratch;
    int enabled;

    /* Keep $at live so the load uses $v0 as both base and destination. */
    enabled = g_DsReadSysEnabled.enabled;
    return enabled;
}
