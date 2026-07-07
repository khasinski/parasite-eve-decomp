/* CC1_FLAGS: -g3 */

extern int g_CdRomCmdTimeout;

int CdRom_SendCmd(int arg0);

int CdRom_IssueSetMode(int arg0) {
    volatile int *ptr;

    ptr = &g_CdRomCmdTimeout;
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    if (*ptr > 0) {
        return 0;
    }
    ptr[-10] = 0x20;
    return CdRom_SendCmd(arg0 & 0xFF);
}
