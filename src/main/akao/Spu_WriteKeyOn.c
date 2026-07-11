typedef unsigned short u16;
typedef unsigned int u32;

extern volatile u16 D_1F801D88;
extern volatile u16 D_1F801D8A;

void Spu_WriteKeyOn(u32 value) {
    D_1F801D88 = value;
    value >>= 16;
    D_1F801D8A = value;
}
