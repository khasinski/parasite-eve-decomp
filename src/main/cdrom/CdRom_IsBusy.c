extern int CD_getsector2(void);

asm(".globl func_80080AE4\n"
    "func_80080AE4 = CdRom_IsBusy");

int CdRom_IsBusy(void) {
    return CD_getsector2() == 0;
}
