/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_cd.h"

extern volatile int g_DsDiskType;

int DsSync(int mode);
int Cd_GetReadyStatus(void);
int DsRead_IsBusy(void);
void DsReadBreak(void);
CdlLOC *CdIntToPos(int i, CdlLOC *p);
int Render_BuildParticleFrame(int com, CdlLOC *pos, int mode, void (*callback)(u_char), int zero);
void GD_cbsync(u_char arg0);

int DsGetDiskType(void) {
    CdlLOC loc;
    int ready;

    if (DsSync(0) == 2) {
        if (DsSync(1) == 0x10) {
            return 0x10;
        }
    }

    while (1) {
        ready = Cd_GetReadyStatus();
        if (ready == 1) {
            break;
        }
        if (Cd_GetReadyStatus() == 3) {
            return 1;
        }
    }

    if (DsRead_IsBusy() != 0) {
        DsReadBreak();
    }

    CdIntToPos(0x10, &loc);
    g_DsDiskType = 0;
    if (Render_BuildParticleFrame(0x20, &loc, 0x1B, GD_cbsync, 0) != 0) {
        while (g_DsDiskType == 0) {
        }
        return g_DsDiskType;
    }
    return 2;
}
