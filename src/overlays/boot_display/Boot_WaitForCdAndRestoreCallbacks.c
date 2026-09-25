#include "common.h"
#include "pe1/boot.h"
#include "pe1/cdrom.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_callbacks.h"

s32 Boot_WaitForCdAndRestoreCallbacks(void) {
    cd_rom4(8, 0, 0);
    while (func_80080DC4(9, 0, 0) == 0) {}
    do {
        while (Cd_GetReadyStatus() != 1) {}
    } while (CdRom_GetPendingReadCount() != 0);
    DsSyncCallback(0);
    VSyncCallback(Boot_VsyncCallback);
    return 0;
}
