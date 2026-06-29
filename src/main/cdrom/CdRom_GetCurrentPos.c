/* MASPSX_FLAGS: --stack-return-delay */

typedef struct {
    char bytes[4];
} UnalignedWord;

UnalignedWord *CdRom_GetCurrentPosPtr(void);

asm(".globl CdRom_GetCurrentPos");
asm("CdRom_GetCurrentPos = func_80080CDC");

UnalignedWord *CdRom_GetCurrentPos(UnalignedWord *dst) __asm__("func_80080CDC");

UnalignedWord *CdRom_GetCurrentPos(UnalignedWord *dst) {
    if (dst != 0) {
        *dst = *CdRom_GetCurrentPosPtr();
        return dst;
    }

    return CdRom_GetCurrentPosPtr();
}
