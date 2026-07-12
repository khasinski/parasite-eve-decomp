typedef unsigned char u8;
typedef unsigned short u16;

extern u16 D_800E2290;
extern u16 D_800E2292;
extern u16 D_800E2294;

int func_800CC244(void *arg0, void *arg1, u8 *anim) {
    u16 z = D_800E2294;

    *(u16 *)(anim + 6) = D_800E2290;
    *(u16 *)(anim + 8) = D_800E2292;
    *(u16 *)(anim + 4) = 0xBE8;
    anim[3] = 0x7F;
    *(u16 *)(anim + 0xA) = z;
}
