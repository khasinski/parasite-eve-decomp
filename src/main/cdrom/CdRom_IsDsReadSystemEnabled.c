extern int g_DsReadSysEnabled;

asm(".globl CdRom_IsDsReadSystemEnabled");
asm("CdRom_IsDsReadSystemEnabled = func_80080940");

int CdRom_IsDsReadSystemEnabled(void) __asm__("func_80080940");

int CdRom_IsDsReadSystemEnabled(void) {
    return g_DsReadSysEnabled;
}
