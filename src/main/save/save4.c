/* MASPSX_FLAGS: --stack-return-delay --store-return-delay */


#include "include_asm.h"
extern int VSync(int arg0);
extern int CdRom_SendQueuedCmd(int arg0);
extern void Save_ProcessDataCallback(void);

extern int D_8009B6C4;

asm("Sys_VSyncTimeout = Sys_VSyncTimeout");

extern int g_CdReadCompleteCallback;

extern void CdRom_ResetFileDescriptors(void);

extern void DS_read_cbready(void);
extern void DsDataCallback(void *callback);
extern void CdRom_IssueSetMode(int arg0, int arg1) asm("func_8007FCBC");
extern void Render_AllocParticleNode(int arg0, void *arg1, void *arg2, int arg3);

extern int g_CdReadInProgress;

asm(".globl func_80081268");
asm("func_80081268 = Save_ProcessDataCallback");

extern int D_8009B6B8;

int Sys_VSyncTimeout(int arg0) {
    int v0;
    int s0;
    int *state;

    v0 = VSync(-1);
    state = &D_8009B6C4;
    asm volatile("" : "=r"(state) : "0"(state));

    if ((state[0] + 0x4B0) < v0) {
        Save_ProcessDataCallback();
        s0 = -1;
    } else {
        s0 = state[-4];
    }

    CdRom_SendQueuedCmd(arg0);
    return s0;
}

int Save_SetDataCallback(int arg0) {
    int old = g_CdReadCompleteCallback;
    g_CdReadCompleteCallback = arg0;
    return old;
}

void Save_ProcessDataCallback(void) {
    int *state;

    state = &g_CdReadInProgress;
    asm volatile("" : "=r"(state) : "0"(state));
    if (*state == 1) {
        CdRom_ResetFileDescriptors();
        DS_read_cbready();
        if (state[-5] & 1) {
            DsDataCallback((void *)state[-3]);
        }
        CdRom_IssueSetMode(1, 0);
        Render_AllocParticleNode(9, 0, 0, -1);
    }
    g_CdReadInProgress = 0;
}

void CdRom_SetRetryMode(unsigned int arg0) {
    if (arg0 < 2) {
        D_8009B6B8 = arg0;
    }
}
