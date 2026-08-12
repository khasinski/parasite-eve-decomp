/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "common.h"
#include "pe1/psyq_cd.h"

void CD_flush(void);
int CD_cw(int command, void *params, int arg2, int arg3);
void Util_Copy4(u8 *dst, u8 *src);

int CdRom_SendCmd(int command, u8 *params) {
    int timeout;

    CD_flush();

    g_CdRomEventCommandState.pendingCommand = command;
    if (params != 0) {
        Util_Copy4(g_CdRomEventCommandState.pendingParamBytes, params);
        g_CdRomEventCommandState.pendingParams =
            g_CdRomEventCommandState.pendingParamBytes;
    } else {
        g_CdRomEventCommandState.pendingParams = 0;
    }

    timeout = 30;
    if (g_CdRomCmdLongTimeoutTable[
            g_CdRomEventCommandState.pendingCommand] != 0) {
        timeout = 960;
    }
    g_CdRomCmdTimeout = timeout;
    g_CdRomCmdRetryState = 0;

    if (g_CdRomEventCommandState.pendingCommand == 7) {
        if (g_CdRomEventCommandState.command.read.eventFlags.bit1 == 1) {
            g_CdRomEventCommandState.pendingCommand =
                g_CdRomEventCommandState.command.read.eventFlags.bit1;
            g_CdRomEventCommandState.pendingParams = 0;
        }
    } else if (g_CdRomEventCommandState.pendingCommand == 8) {
        if (g_CdRomEventCommandState.command.read.eventFlags.bit1 != 1) {
            g_CdRomEventCommandState.pendingCommand = 1;
            g_CdRomEventCommandState.pendingParams = 0;
        }
    }

    if (CD_cw(g_CdRomEventCommandState.pendingCommand,
              g_CdRomEventCommandState.pendingParams, 0, 1) == 0) {
        g_CdRomEventCommandState.command.read.lastCommand =
            g_CdRomEventCommandState.pendingCommand;
        return 1;
    }

    g_CdRomCmdRetryState = 0;
    g_CdRomCmdTimeout = 0;
    return 0;
}
