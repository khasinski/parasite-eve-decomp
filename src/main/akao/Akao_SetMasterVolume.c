extern unsigned char *_spu_RXX;
extern short g_SpuMasterVolume[];

void Akao_SetMasterVolume(short left, short right) {
    short *vol;

    *(short *)(_spu_RXX + 0x184) = left;
    *(short *)(_spu_RXX + 0x186) = right;
    asm volatile("" : "=r"(vol) : "0"(g_SpuMasterVolume));
    vol[0] = left;
    vol[1] = right;
}
