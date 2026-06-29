extern void CD_ready(int, int);

asm(".globl CdRom_SendReadyCommand");
asm("CdRom_SendReadyCommand = func_8007FC88");

void CdRom_SendReadyCommand(int result) __asm__("func_8007FC88");

void CdRom_SendReadyCommand(int result) {
    CD_ready(1, result);
}
