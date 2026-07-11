typedef unsigned short u16;
typedef unsigned int u32;

extern volatile u16 D_1F801D8C;
extern volatile u16 D_1F801D8E;

void Spu_WriteKeyOff(u32 value) {
    D_1F801D8C = value;
    value >>= 16;
    D_1F801D8E = value;
}
