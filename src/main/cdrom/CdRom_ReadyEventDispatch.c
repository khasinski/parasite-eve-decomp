/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */
#include "common.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"

void CdRom_ProcessEventByte(int event, u8 *data);

void CdRom_ReadyEventDispatch(int event, u8 *data) {
    int event_reg;
    u8 event_arg;
    u8 *data_reg;
    CdRomCommandState *state;
    u32 pending;
    data_reg = data;
    event_reg = event & 0xFF;
    CdRom_ProcessEventByte(event_reg, data_reg);

    state = &g_CdSeekState;
    if (state->eventStatus & 0x10) {
        state->read.status = 2;
        state->read.command = 0xC;
    }

    if (g_DsReadyCallback != 0) {
        pending = ((CdRomSystemState *)((char *)state -
            CDROM_SYSTEM_COMMAND_OFFSET))->enabled;
        event_arg = event_reg;
        if (pending != 0) {
            g_DsReadyCallback(event_arg, data_reg);
        }
    }
}
