/* CC1_FLAGS: -g3 -G0 */
/* MASPSX_FLAGS: -G0 */
typedef int s32;

extern int g_CdRomCmdTimeout;
s32 CdRom_SendCmd(s32 arg0);

s32 CdRom_TryIssueCmd(s32 arg0) {
    register int *base asm("$6");
    register int *prev asm("$7");

    base = &g_CdRomCmdTimeout;
    asm volatile("" : "=r"(base) : "0"(base));
    if (base[0] > 0) {
        return 0;
    }
    if (base[-0x11] == 0) {
        prev = base - 0x11;
        return 0;
    }
    prev = base - 0x11;
    asm volatile("" : "=r"(prev) : "0"(prev));
    if (base[-9] != 1) {
        return 0;
    }
    prev[7] = 0x1F;
    base[-9] = 2;
    return CdRom_SendCmd((prev[9] = 0xB, arg0 & 0xFF));
}
