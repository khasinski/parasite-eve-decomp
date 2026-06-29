extern volatile unsigned short D_1F801D98;
extern volatile unsigned short D_1F801D9A;

void Spu_WriteReverbEnable(unsigned int value) {
    D_1F801D98 = value;
    value >>= 16;
    D_1F801D9A = value;
}
