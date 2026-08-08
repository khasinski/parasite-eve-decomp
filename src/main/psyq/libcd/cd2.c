/* MASPSX_FLAGS: --la-return-delay */

#include "pe1/psyq_cd.h"

extern int g_CdStreamRingSize;
extern int g_CdStreamRingWritePtr;
extern int g_CdStreamRingReadSlot;
extern int D_800BE998;
extern int g_CdStreamDataReadyFlag;
extern int D_800B0CD0;
extern short g_StStreamState;
extern int g_StStreamReadState;

extern void init_ring_status(int arg0, int arg1);

extern int EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void DsDataCallback(void *callback);
extern void DsSyncCallback(void *callback);
extern void CdDataCallback(void *callback);
extern void CdReadyCallback(void *callback);

extern int D_8009AFD8;
extern unsigned char *D_8009AF1C;
extern unsigned char *D_8009AF28;

extern unsigned char g_CdStatus;

extern unsigned char g_CdMode;

extern unsigned char g_CdLastCom;

extern CdlLOC g_CdLastPos;

void StClearRing(void) {
    int arg1 = g_CdStreamRingSize;

    g_CdStreamRingWritePtr = 0;
    g_CdStreamRingReadSlot = 0;
    D_800BE998 = 0;
    g_CdStreamDataReadyFlag = 0;
    init_ring_status(0, arg1);
    D_800B0CD0 = 0;
    g_StStreamState = 0;
    g_StStreamReadState = 0;
}

void CdRom_Shutdown(void) {
    unsigned char *ptr;

    EnterCriticalSection();
    if (D_8009AFD8 == 1) {
        DsDataCallback(0);
        DsSyncCallback(0);
    } else {
        CdDataCallback(0);
        CdReadyCallback(0);
    }
    *D_8009AF1C = 0;
    ptr = D_8009AF28;
    asm volatile("nop");
    *ptr = 0;
    ExitCriticalSection();
}

int CdStatus(void) {
    return g_CdStatus;
}

int CdMode(void) {
    return g_CdMode;
}

int CdLastCom(void) {
    return g_CdLastCom;
}

CdlLOC *CdLastPos(void) {
    return &g_CdLastPos;
}
