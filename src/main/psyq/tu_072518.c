/* MASPSX_FLAGS: --stack-return-delay */

#include "include_asm.h"

#include "pe1/psyq_cd.h"

CdlLOC *CdIntToPos(int i, CdlLOC *p);
int CdRom_StartRead(CdlLOC *loc, int arg1, int arg2, int mode);
int Sys_VSyncTimeout(int arg0);

extern int g_DsReadBusy;

void DsSyncCallback(int arg0);
void DsReadyCallback(int arg0);
int Render_AllocParticleNode(int com, void *param, void *result, int mode);

typedef void (*DsCallback)(void);

extern DsCallback D_8009B708;

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

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/tu_072518", CdRom_InitAsyncRead);

void DsReadBreak(void) {
    int *state;

    state = &g_DsReadBusy;
    asm volatile("" : "=r"(state) : "0"(state));
    if (*state == 1) {
        DsSyncCallback(state[-3]);
        DsReadyCallback(state[-2]);
        Render_AllocParticleNode(9, 0, 0, -1);
    }
    *state = 0;
}

DsCallback DsReadCallback(DsCallback callback) {
    DsCallback *slot;
    DsCallback old;

    slot = &D_8009B708;
    old = *slot;
    *slot = callback;
    return old;
}
