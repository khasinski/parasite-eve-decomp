/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"

int CdRom_StartRead(CdlLOC *position, int sectors, void *destination, int mode) {
    CdlLOC location;
    CdReadProgressState *state = &g_CdReadProgress;
    int result;
    if (state->inProgress == 1 || DsRead_IsBusy()) return 0;
    state->sectorSize = 512;
    state->destination = (int)destination;
    state->remainingSectors = sectors;
    if (!position) location = *CdRom_GetCurrentPos(0);
    else location = *position;
    mode |= 0x20;
    result = Render_BuildParticleFrame((u8)mode, &location, 6, CdRom_SetMode2Callback, -1);
    if (!result) return 0;
    state->startVsync = VSync(-1);
    if (state->flags & 1)
        state->dataCallback = DsDataCallback(Render_StepParticleCallback);
    state->inProgress = 1;
    return result;
}
