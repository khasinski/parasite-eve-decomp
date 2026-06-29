extern volatile unsigned short D_1F801D94;
extern volatile unsigned short D_1F801D96;

void Spu_WriteNoiseEnable(unsigned int value) {
    D_1F801D94 = value;
    value >>= 16;
    D_1F801D96 = value;
}
