/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "include_asm.h"

#include "pe1/psyq_cd.h"

CdlLOC *CdIntToPos(int i, CdlLOC *p);
int CdRom_StartRead(CdlLOC *loc, int arg1, int arg2, int mode);
int Sys_VSyncTimeout(int arg0);

extern int g_DsReadBusy;

int DsSyncCallback(int callback);
int DsReadyCallback(int callback);
int Render_AllocParticleNode(int com, void *param, void *result, int mode);

int ds_read(int arg0, int arg1, int arg2) {
    CdlLOC loc;
    int status;

    CdIntToPos(arg1, &loc);
    CdRom_StartRead(&loc, arg0, arg2, 0x80);

    do {
        status = Sys_VSyncTimeout(0);
    } while (status > 0);

    return status < 1U;
}

void CdRom_AsyncCallback(void);
void CdRom_ReadDoneCallback(void);

int CdRom_InitAsyncRead(DsCallback callback, int arg1) {
    int *state;

    state = &g_DsReadBusy;
    if (*state == 1) {
        return 0;
    }

    state[-8] = -1;
    state[-7] = 0;
    state[-5] = 0;
    state[-6] = (int)callback;
    state[-4] = arg1;
    state[-3] = DsSyncCallback((int)CdRom_AsyncCallback);
    state[-2] = DsReadyCallback((int)CdRom_ReadDoneCallback);
    *state = 1;
    return 1;
}

void DsReadBreak(void) {
    int *state;

    state = &g_DsReadBusy;
    if (*state == 1) {
        DsSyncCallback(state[-3]);
        DsReadyCallback(state[-2]);
        Render_AllocParticleNode(9, 0, 0, -1);
    }
    *state = 0;
}
