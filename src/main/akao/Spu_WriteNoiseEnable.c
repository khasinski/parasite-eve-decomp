typedef unsigned short u16;
typedef unsigned int u32;

extern volatile u16 D_1F801D94;
extern volatile u16 D_1F801D96;

void Spu_WriteNoiseEnable(u32 value) {
    D_1F801D94 = value;
    value >>= 16;
    D_1F801D96 = value;
}
