/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSREAD.OBJ: DsRead. */

#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds_queue.h"
#include "pe1/cdrom.h"

#define READ_STATE(anchor, field) ((CdReadProgressState *)((char *)(anchor) - PE1_OFFSETOF(CdReadProgressState, field)))

int DsRead(CdlLOC *position, int sectors, void *destination, int incomingMode) {
    register int incoming asm("$7") = incomingMode;
    int mode;
    CdlLOC location;
    int *state;
    register int result asm("$2");
    /* Empty constraints preserve the retail saves and incoming a3 lifetime. */
    asm("" : "=r"(incoming) : "0"(incoming) : "$17", "$16");
    state = &g_CdReadInProgress;
    asm volatile("" : "=r"(state), "=r"(incoming) : "0"(state), "1"(incoming));
    mode = incoming;
    if (*state != 1) {
        if (!ER_active()) goto start;
    }
    result = 0;
    goto done;
start:
    READ_STATE(state, inProgress)->sectorSize = 512;
    READ_STATE(state, inProgress)->destination = (int)destination;
    READ_STATE(state, inProgress)->remainingSectors = sectors;
    /* GCC merges these calls after scheduling the two position copies. */
    if (!position) {
        location = *DsLastPos(0);
        mode |= 0x20;
        mode = DsPacket((u8)mode, &location, 6,
                                         (DslCB)DS_read_cbsync, -1);
    } else {
        location = *position;
        mode |= 0x20;
        mode = DsPacket((u8)mode, &location, 6,
                                         (DslCB)DS_read_cbsync, -1);
    }
    result = 0;
    if (!mode) goto done;
    result = VSync(-1);
    state = &g_CdReadStartVsync;
    asm volatile("" : "=r"(state) : "0"(state));
    *state = result;
    if (READ_STATE(state, startVsync)->flags & 1)
        READ_STATE(state, startVsync)->dataCallback = DsDataCallback(DS_read_cbdata);
    READ_STATE(state, startVsync)->inProgress = 1;
    result = mode;
done:
    return result;
}

#undef READ_STATE
