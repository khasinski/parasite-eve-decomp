extern volatile unsigned short D_1F801D90;
extern volatile unsigned short D_1F801D92;

void Spu_WriteFmEnable(unsigned int value) {
    D_1F801D90 = value;
    value >>= 16;
    D_1F801D92 = value;
}
