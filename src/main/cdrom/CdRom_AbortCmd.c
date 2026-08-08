/* MASPSX_FLAGS: --stack-return-delay */

#include "include_asm.h"
#include "pe1/psyq_cd.h"

typedef unsigned int u32;

extern u32 D_8009B554[];
extern DsReadStatusBlock g_DsReadStatusBlock asm("D_8009B574");

void CD_flush(void);

void CdRom_AbortCmd(void) {
    u32 *state;
    u32 kind;
    u32 cmp;
    state = D_8009B554;
    asm volatile("" : "=r"(state) : "0"(state));
    state[0] = 0;
    CD_flush();

    if (state[8] == 2) {
        kind = state[9];
        cmp = 0xB;
        if (kind == cmp) {
            goto abort_pending;
        }
        cmp = 0x11;
        if (kind == cmp) {
            goto abort_pending;
        }
        cmp = 0x10;
        if (kind == cmp) {
            DsReadStatusBlock *slot;
            u32 value;
abort_pending:
            slot = &g_DsReadStatusBlock;
            asm("" : "=r"(slot) : "0"(slot));
            value = 1;
            slot->status = value;
            value = 0xB;
            slot->command = value;
        }
    }
}
