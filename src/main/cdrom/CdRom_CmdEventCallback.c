#include "common.h"
#include "pe1/psyq_cd.h"

extern CdRomEventCommandState D_8009B558_o __asm__("D_8009B558");

#define D_8009B558 (&D_8009B558_o)

void CdRom_CmdEventCallback(int event, u8 *result) {
    u32 event_reg;
    register CdRomEventCommandState *cmd_state asm("$6");
    int value;
    int status;
    CdRomCommandState *ready;

    event_reg = event & 0xFF;
    if (event_reg == 2) {
        cmd_state = D_8009B558;
        asm volatile("" : "=r"(cmd_state) : "0"(cmd_state));
        status = cmd_state->pendingCommand;
        value = 0xE;
        if (status == value) {
            value = cmd_state->command.read.commandMode;
            status = cmd_state->pendingParamBytes[0];
            if (((value ^ status) & 0x80) != 0) {
                cmd_state->command.read.command = 0xF;
                cmd_state->command.read.status = event_reg;
                cmd_state->command.read.syncResult = 3;
            }
            cmd_state->command.read.commandMode = status;
        }
    }

    status = event & 0xFF;
    if (status == 5) {
        ready = &g_CdSeekState;
        asm volatile("" : "=r"(ready) : "0"(ready));
        if (ready->eventStatus & 0x10) {
            ready->read.status = 2;
            ready->read.command = 12;
            if (g_DsSyncCallback &&
                ((CdRomSystemState *)((char *)ready -
                    PE1_OFFSETOF(CdRomSystemState, command)))->enabled) {
                g_DsSyncCallback(5, result);
            }
        } else {
            ready->read.status = 1;
            ready->read.command = 11;
        }
    }
}
