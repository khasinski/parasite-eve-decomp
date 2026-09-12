/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"
extern int VSync(int mode);
extern int g_CdReadCurrentVsync __asm__("D_8009B6C8");
extern CdReadCompleteCallback D_8009B6D0;

#define CD_READ_FIELD(anchor, field)                                      \
    ((anchor)[(PE1_OFFSETOF(CdReadProgressState, field) -                  \
               PE1_OFFSETOF(CdReadProgressState, currentVsync)) /         \
              sizeof(int)])

void CdRom_ReadProgressCallback(int status, void *data, void *detail) {
    int savedStatus = status;
    int *state = &g_CdReadCurrentVsync;
    CD_READ_FIELD(state, currentVsync) = VSync(-1);
    if (CD_READ_FIELD(state, flags) & 1) {
        if (CD_READ_FIELD(state, remainingSectors) > 0) {
            CdRom_IsBusy2(CD_READ_FIELD(state, destination),
                          CD_READ_FIELD(state, sectorSize));
            CD_READ_FIELD(state, eventData) = (int)data;
        } else {
            Save_ProcessDataCallback();
            if (D_8009B6D0) {
                if (CD_READ_FIELD(state, remainingSectors) < 0) savedStatus = 5;
                D_8009B6D0((u8)savedStatus, (int)data);
            }
        }
    } else {
        if (CD_READ_FIELD(state, remainingSectors) > 0) {
            CdRom_IsBusy(CD_READ_FIELD(state, destination),
                         CD_READ_FIELD(state, sectorSize));
            CD_READ_FIELD(state, destination) +=
                CD_READ_FIELD(state, sectorSize) * 4;
            CD_READ_FIELD(state, remainingSectors)--;
        }
        if (VSync(-1) > CD_READ_FIELD(state, startVsync) + 1200)
            CD_READ_FIELD(state, remainingSectors) = -1;
        if (CD_READ_FIELD(state, remainingSectors) == 0 ||
            VSync(-1) > CD_READ_FIELD(state, startVsync) + 1200) {
            Save_ProcessDataCallback();
            if (D_8009B6D0) {
                savedStatus =
                    CD_READ_FIELD(state, remainingSectors) < 0 ? 5 : 2;
                D_8009B6D0((u8)savedStatus, (int)data);
            }
        }
    }
}

#undef CD_READ_FIELD
