#include "common.h"
#include "pe1/boot.h"
#include "pe1/cdrom.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"
#include "pe1/psyq_callbacks.h"

s32 Boot_WaitForCdAndRestoreCallbacks(void) {
    DsControl(8, 0, 0);
    while (DsControlB(9, 0, 0) == 0) {}
    do {
        while (DsSystemStatus() != 1) {}
    } while (DsQueueLen() != 0);
    DsSyncCallback(0);
    VSyncCallback(Boot_VsyncCallback);
    return 0;
}
