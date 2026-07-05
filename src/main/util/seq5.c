extern unsigned int D_8009B3A0;

extern unsigned short D_8009D2B6;
extern int D_800C0D90;
extern unsigned short D_800C0DA0;
extern unsigned short D_800C0DA2;
extern int D_800C0DA4;

extern void SPU_WriteVoiceRegs(void);

void Seq_GetGlobalPitch(unsigned int *out)
{
    *out = D_8009B3A0;
}

void Seq_ApplyGlobalPitch(void) {
    unsigned short value = D_8009D2B6;
    register char *regs asm("$4");

    __asm__("" : "=r"(regs) : "0"(&D_800C0D90));
    *(int *)regs = 0x1C0;
    D_800C0DA4 = 0;
    D_800C0DA2 = value;
    D_800C0DA0 = value;
    SPU_WriteVoiceRegs();
}

void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size)
{
    size >>= 2;
    do {
        *dst = *src;
        src++;
        size--;
        dst++;
    } while (size != 0);
}
