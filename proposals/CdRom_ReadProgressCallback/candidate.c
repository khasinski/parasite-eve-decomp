/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"
extern int VSync(int mode);

void CdRom_ReadProgressCallback(int status, void *data, void *detail) {
    CdReadProgressState *state = &g_CdReadProgress;
    state->currentVsync = VSync(-1);
    if (state->flags & 1) {
        if (state->remainingSectors > 0) {
            CdRom_IsBusy2(state->destination, state->sectorSize);
            state->eventData = (int)data;
        } else {
            Save_ProcessDataCallback();
            if (g_CdReadCompleteCallback) {
                if (state->remainingSectors < 0) status = 5;
                g_CdReadCompleteCallback((u8)status, (int)data);
            }
        }
    } else {
        if (state->remainingSectors > 0) {
            CdRom_IsBusy(state->destination, state->sectorSize);
            state->destination += state->sectorSize * 4;
            state->remainingSectors--;
        }
        if (VSync(-1) > state->startVsync + 1200)
            state->remainingSectors = -1;
        if (state->remainingSectors == 0 || VSync(-1) > state->startVsync + 1200) {
            Save_ProcessDataCallback();
            if (g_CdReadCompleteCallback) {
                status = state->remainingSectors < 0 ? 5 : 2;
                g_CdReadCompleteCallback((u8)status, (int)data);
            }
        }
    }
}
