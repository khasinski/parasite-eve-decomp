extern volatile unsigned short D_1F801D88;
extern volatile unsigned short D_1F801D8A;

void Spu_WriteKeyOn(unsigned int value) {
    D_1F801D88 = value;
    value >>= 16;
    D_1F801D8A = value;
}
