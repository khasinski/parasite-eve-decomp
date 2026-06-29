extern int CD_getsector(void);

asm(".globl func_80080B04\n"
    "func_80080B04 = CdRom_IsBusy2");

int CdRom_IsBusy2(void) {
    return CD_getsector() == 0;
}
