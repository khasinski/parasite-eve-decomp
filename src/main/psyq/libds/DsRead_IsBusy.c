extern int g_DsReadBusy;

asm(".globl DsRead_IsBusy");
asm("DsRead_IsBusy = func_800822AC");

int DsRead_IsBusy(void) __asm__("func_800822AC");

int DsRead_IsBusy(void) {
    return g_DsReadBusy;
}
