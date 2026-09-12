/* ASSEMBLER: GNU */

#include "pe1/psyq_cd.h"

extern int D_8009B6CC;
extern int D_8009B6C4;
#define READ_STATE(anchor, field) ((CdReadProgressState *)((char *)(anchor) - PE1_OFFSETOF(CdReadProgressState, field)))

int CdRom_StartRead(CdlLOC *position, int sectors, void *destination, int incomingMode) {
    register int incoming asm("$7") = incomingMode;
    int mode;
    CdlLOC location;
    int *state;
    register int result asm("$2");
    /* Empty constraints preserve the retail saves and incoming a3 lifetime. */
    asm("" : "=r"(incoming) : "0"(incoming) : "$17", "$16");
    state = &D_8009B6CC;
    asm volatile("" : "=r"(state), "=r"(incoming) : "0"(state), "1"(incoming));
    mode = incoming;
    if (*state != 1) {
        if (!DsRead_IsBusy()) goto start;
    }
    /* Keep the shared rejection block instead of threading both guards. */
    asm volatile("" : : : "memory");
    result = 0;
    goto done;
start:
    READ_STATE(state, inProgress)->sectorSize = 512;
    READ_STATE(state, inProgress)->destination = (int)destination;
    READ_STATE(state, inProgress)->remainingSectors = sectors;
    /* GCC merges these calls after scheduling the two position copies. */
    if (!position) {
        location = *CdRom_GetCurrentPos(0);
        mode |= 0x20;
        mode = Render_BuildParticleFrame((u8)mode, &location, 6, CdRom_SetMode2Callback, -1);
    } else {
        location = *position;
        mode |= 0x20;
        mode = Render_BuildParticleFrame((u8)mode, &location, 6, CdRom_SetMode2Callback, -1);
    }
    result = 0;
    if (!mode) goto done;
    result = VSync(-1);
    state = &D_8009B6C4;
    asm volatile("" : "=r"(state) : "0"(state));
    *state = result;
    if (READ_STATE(state, startVsync)->flags & 1)
        READ_STATE(state, startVsync)->dataCallback = DsDataCallback(Render_StepParticleCallback);
    READ_STATE(state, startVsync)->inProgress = 1;
    result = mode;
done:
    return result;
}

#undef READ_STATE
