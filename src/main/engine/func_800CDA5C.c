typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);

extern u16 D_800E27F0;
extern u16 D_800E27F2;
extern u16 D_800E27F4;

int func_800CDA5C(void *arg0, void *arg1, u8 *anim) {
    u16 *base = &D_800E27F0;
    u16 *base_s4;
    int i = 0;
    u8 *entry;
    int value;

    *(u16 *)(anim + 0x4) = base[0];
    *(u16 *)(anim + 0x6) = D_800E27F2;
    *(u16 *)(anim + 0x8) = D_800E27F4;
    *(u16 *)(anim + 0xA) = (rand() % 201) - 0x64;
    *(u16 *)(anim + 0xC) = (rand() % 101) - 0x32;

    base_s4 = base;
    anim[1] = 0x7F;
    *(u16 *)(anim + 0xE) = 0;
    anim[3] = 0;

    do {
        entry = (u8 *)(((i & 0xFFFF) * 2) + (int)anim);
        *(u16 *)(entry + 0x10) = base_s4[0];
        *(u16 *)(entry + 0x20) = base_s4[1];
        *(u16 *)(entry + 0x30) = base_s4[2];
        *(u16 *)(entry + 0x40) = (rand() % 50) - 0x19;
        value = (rand() % 50) - 0x19;
        i++;
        *(u16 *)(entry + 0x60) = 0;
        *(u16 *)(entry + 0x50) = value;
    } while ((unsigned int)(i & 0xFFFF) < 8U);
}
