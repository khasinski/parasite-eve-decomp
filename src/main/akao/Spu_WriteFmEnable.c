typedef unsigned short u16;
typedef unsigned int u32;

extern volatile u16 D_1F801D90;
extern volatile u16 D_1F801D92;

void Spu_WriteFmEnable(u32 value) {
    D_1F801D90 = value;
    value >>= 16;
    D_1F801D92 = value;
}
