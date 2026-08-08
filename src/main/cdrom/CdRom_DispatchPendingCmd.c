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
    CdRomPendingCmd *entry;
    register s32 offset asm("$2");
    s32 result;
    if (DsSync(0) != 1) {
        return 0;
    }

    entry = (CdRomPendingCmd *)g_CdDsReadIndex;
    index = *(s32 *)entry;
    entry = (CdRomPendingCmd *)((u8 *)entry - 0xC4);
    offset = index * 3;
    offset <<= 3;
    entry = (CdRomPendingCmd *)((u8 *)entry + offset);

    asm volatile(
        ".set\tnoreorder\n\t"
        "lw\t%0,0x0(%1)\n\t"
        "nop\n\t"
        "beqz\t%0,1f\n\t"
        "addu\t%0,$zero,$zero\n\t"
        "lbu\t$4,0x4(%1)\n\t"
        "lw\t$5,0xC(%1)\n\t"
        "jal\tCdRom_TryIssueCmd\n\t"
        "nop\n\t"
        "sltu\t%0,$zero,$2\n"
        "1:\n\t"
        ".set\treorder"
        : "=r"(result)
        : "r"(entry)
        : "$4", "$5", "$31", "memory");

    return result;
}
