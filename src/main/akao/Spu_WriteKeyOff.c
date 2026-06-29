extern volatile unsigned short D_1F801D8C;
extern volatile unsigned short D_1F801D8E;

void Spu_WriteKeyOff(unsigned int value) {
    D_1F801D8C = value;
    value >>= 16;
    D_1F801D8E = value;
}
