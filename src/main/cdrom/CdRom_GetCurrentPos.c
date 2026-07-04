/* MASPSX_FLAGS: --stack-return-delay */

typedef struct {
    char bytes[4];
} UnalignedWord;

UnalignedWord *CdRom_GetCurrentPosPtr(void);

UnalignedWord *CdRom_GetCurrentPos(UnalignedWord *dst);

UnalignedWord *CdRom_GetCurrentPos(UnalignedWord *dst) {
    if (dst != 0) {
        *dst = *CdRom_GetCurrentPosPtr();
        return dst;
    }

    return CdRom_GetCurrentPosPtr();
}
