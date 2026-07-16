typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);

extern u16 D_800E27F0;
extern u16 D_800E27F2;
extern u16 D_800E27F4;

int func_800CDA5C(void *arg0, void *arg1, u8 *anim) {
    u16 *base_s4 = &D_800E27F0;
    int i = 0;
    u8 *entry;

    *(u16 *)(anim + 0x4) = base_s4[0];
    *(u16 *)(anim + 0x6) = base_s4[1];
    *(u16 *)(anim + 0x8) = base_s4[2];
    *(u16 *)(anim + 0xA) = (rand() % 201) - 0x64;
    anim[1] = 0x7F;
    *(u16 *)(anim + 0xE) = 0;
    anim[3] = 0;
    *(u16 *)(anim + 0xC) = (rand() % 101) - 0x32;

    do {
        entry = anim + ((i & 0xFFFF) * 2);
        *(u16 *)(entry + 0x10) = base_s4[0];
        *(u16 *)(entry + 0x20) = base_s4[1];
        *(u16 *)(entry + 0x30) = base_s4[2];
        *(u16 *)(entry + 0x40) = (rand() % 50) - 0x19;
        i++;
        *(u16 *)(entry + 0x60) = 0;
        *(u16 *)(entry + 0x50) = (rand() % 50) - 0x19;
    } while ((i & 0xFFFF) < 8);
}
