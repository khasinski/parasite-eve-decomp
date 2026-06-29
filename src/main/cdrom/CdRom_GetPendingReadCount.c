extern int g_CdPendingReadCount;

asm(".globl CdRom_GetPendingReadCount");
asm("CdRom_GetPendingReadCount = func_8007F778");

int CdRom_GetPendingReadCount(void) __asm__("func_8007F778");

int CdRom_GetPendingReadCount(void) {
    return g_CdPendingReadCount;
}
