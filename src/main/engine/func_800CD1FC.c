typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);

extern u16 D_800E27F8;
extern u16 D_800E27FA;
extern u16 D_800E27FC;

int func_800CD1FC(void *arg0, void *arg1, u8 *anim) {
    u16 z;

    *(u16 *)(anim + 0x8) = (D_800E27F8 + (rand() % 201)) - 0x64;
    *(u16 *)(anim + 0xA) = (D_800E27FA + (rand() % 101)) - 0x32;
    z = D_800E27FC;
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0x544;
    *(u16 *)(anim + 0xC) = z;
}
