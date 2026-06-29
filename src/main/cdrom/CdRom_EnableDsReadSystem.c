/* MASPSX_FLAGS: --store-return-delay */
/* CC1_PROFILE: build0001 */
extern int g_DsReadSysEnabled;

asm(".globl CdRom_EnableDsReadSystem");
asm("CdRom_EnableDsReadSystem = func_80080930");

void CdRom_EnableDsReadSystem(void) __asm__("func_80080930");

void CdRom_EnableDsReadSystem(void) {
    g_DsReadSysEnabled = 1;
}
