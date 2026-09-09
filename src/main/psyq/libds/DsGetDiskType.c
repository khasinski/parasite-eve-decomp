/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"

u32 DsSync(u32 mode);
int Cd_GetReadyStatus(void);
void DsReadBreak(void);
CdlLOC *CdIntToPos(int sector, CdlLOC *position);
void GD_cbsync(unsigned char event);

int DsGetDiskType(void) {
    CdlLOC pos;

    if (DsSync(0) == 2 && DsSync(1) == 16) {
        /* Prevent copying the known comparison value from v1 into v0. */
        asm volatile("" : : : "$3");
        return 16;
    }
    while (Cd_GetReadyStatus() != 1) {
        if (Cd_GetReadyStatus() == 3) {
            return 1;
        }
    }
    if (DsRead_IsBusy()) {
        DsReadBreak();
    }
    CdIntToPos(16, &pos);
    g_DsDiskType = 0;
    if (!Render_BuildParticleFrame(32, &pos, 27, GD_cbsync, 0)) {
        return 2;
    }
    while (!g_DsDiskType) {
    }
    return g_DsDiskType;
}
