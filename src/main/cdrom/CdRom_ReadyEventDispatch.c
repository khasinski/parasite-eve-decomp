/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */
#include "common.h"
#include "pe1/psyq_cd.h"

void CdRom_ProcessEventByte(int event);

void CdRom_ReadyEventDispatch(int event, u8 *data) {
    int event_reg;
    register int event_arg asm("$4");
    u8 *data_reg;
    CdRomCommandState *state;
    u32 pending;
    data_reg = data;
    event_reg = event & 0xFF;
    CdRom_ProcessEventByte(event_reg);

    state = &g_CdSeekState;
    if (state->eventStatus & 0x10) {
        state->read.status = 2;
        state->read.command = 0xC;
    }

    if (g_DsReadyCallback != 0) {
        pending = ((CdRomSystemState *)((char *)state -
            PE1_OFFSETOF(CdRomSystemState, command)))->enabled;
        if (pending != 0) {
            event_arg = event_reg;
            asm volatile("" : : "r"(event_arg));
            g_DsReadyCallback(event_arg, data_reg);
        }
    }
}
