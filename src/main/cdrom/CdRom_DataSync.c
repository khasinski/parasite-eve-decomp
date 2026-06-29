extern int CD_datasync(void);

asm(".globl CdRom_DataSync");
asm("CdRom_DataSync = func_80080B24");

int CdRom_DataSync(void) __asm__("func_80080B24");

int CdRom_DataSync(void) {
    return CD_datasync();
}
