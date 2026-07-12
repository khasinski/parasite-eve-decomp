typedef unsigned char u8;
typedef unsigned short u16;

extern u16 D_800E27F8;
extern u16 D_800E27FC;
extern char *D_8009D254;

int func_800CD07C(void *arg0, void *arg1, u8 *anim) {
    u16 x = D_800E27F8;
    char *entity = D_8009D254;
    u16 z = D_800E27FC;

    *(u16 *)(anim + 8) = x;
    *(u16 *)(anim + 0xA) = *(short *)(entity + 0x2E);
    *(u16 *)(anim + 4) = 0x7F;
    *(u16 *)(anim + 6) = 0x60C;
    *(u16 *)(anim + 0xC) = z;
}
