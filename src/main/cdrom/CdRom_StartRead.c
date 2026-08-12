/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/psyq_cd.h"

int CdRom_StartRead(CdlLOC *location, int size, int destination, int mode)
{
    CdlLOC startLocation;
    int result;

    if (g_CdReadProgress.inProgress == 1) {
        return 0;
    }
    if (DsRead_IsBusy() != 0) {
        return 0;
    }

    g_CdReadProgress.sectorSize = 0x200;
    g_CdReadProgress.destination = destination;
    g_CdReadProgress.remainingSectors = size;

    if (location == 0) {
        startLocation = *CdRom_GetCurrentPos(0);
    } else {
        startLocation = *location;
    }
    mode |= 0x20;

    result = Render_BuildParticleFrame(mode & 0xFF, &startLocation, 6,
                                       CdRom_SetMode2Callback, -1);
    if (result == 0) {
        return 0;
    }

    g_CdReadProgress.startVsync = VSync(-1);
    if (g_CdReadProgress.flags & 1) {
        g_CdReadProgress.callbackToken =
            DsDataCallback(Render_StepParticleCallback);
    }
    g_CdReadProgress.inProgress = 1;

    return result;
}
