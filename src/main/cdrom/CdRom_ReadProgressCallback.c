/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/psyq_cd.h"

void CdRom_ReadProgressCallback(int event, int data)
{
    CdReadCompleteCallback callback;
    int mode;

    g_CdReadProgress.currentVsync = VSync(-1);

    if (g_CdReadProgress.flags & 1) {
        if (g_CdReadProgress.remainingSectors > 0) {
            CdRom_IsBusy2(g_CdReadProgress.destination,
                          g_CdReadProgress.sectorSize);
            g_CdReadProgress.eventData = data;
            return;
        }

        Save_ProcessDataCallback();
        callback = g_CdReadCompleteCallback;
        if (callback == 0) {
            return;
        }
        mode = event;
    } else {
        if (g_CdReadProgress.remainingSectors > 0) {
            CdRom_IsBusy(g_CdReadProgress.destination,
                         g_CdReadProgress.sectorSize);
            g_CdReadProgress.destination +=
                g_CdReadProgress.sectorSize << 2;
            g_CdReadProgress.remainingSectors--;
        }

        if (g_CdReadProgress.startVsync + 0x4B0 < VSync(-1)) {
            g_CdReadProgress.remainingSectors = -1;
        }

        if (g_CdReadProgress.remainingSectors != 0) {
            if (g_CdReadProgress.startVsync + 0x4B0 >= VSync(-1)) {
                return;
            }
        }

        Save_ProcessDataCallback();
        callback = g_CdReadCompleteCallback;
        if (callback == 0) {
            return;
        }
        mode = 2;
    }

    if (g_CdReadProgress.remainingSectors < 0) {
        mode = 5;
    }
    callback(mode & 0xFF, data);
}
