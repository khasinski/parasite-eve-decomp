extern void CD_sync(int, int);

asm(".globl CdRom_Sync");
asm("CdRom_Sync = func_8007FC64");

void CdRom_Sync(int result) __asm__("func_8007FC64");

void CdRom_Sync(int result) {
    CD_sync(1, result);
}
