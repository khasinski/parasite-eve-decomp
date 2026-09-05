#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef struct CdRomPendingCmd {
    s32 active;
    u8 cmd;
    u8 pad5[7];
    s32 arg;
    u8 pad10[8];
} CdRomPendingCmd;

extern s32 g_CdDsReadIndex[] __asm__("D_800A3604");

s32 DsSync(s32 mode);
s32 CdRom_TryIssueCmd(s32 cmd, s32 arg);

s32 CdRom_DispatchPendingCmd(void) {
    s32 index;
    register CdRomPendingCmd *entry asm("$3");
    register s32 offset asm("$2");
    if (DsSync(0) != 1) {
        return 0;
    }

    entry = (CdRomPendingCmd *)g_CdDsReadIndex;
    index = *(s32 *)entry;
    entry = (CdRomPendingCmd *)((u8 *)entry - 0xC4);
    offset = index * 3;
    offset <<= 3;
    entry = (CdRomPendingCmd *)((u8 *)entry + offset);

    if (entry->active != 0) {
        return CdRom_TryIssueCmd(entry->cmd, entry->arg) != 0;
    }
    return 0;
}
