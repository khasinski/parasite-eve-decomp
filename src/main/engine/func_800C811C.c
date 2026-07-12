typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

extern char *D_8009D254;
extern u16 D_800E2348;
extern u16 D_800E234C;

int func_800C811C(void *arg0, void *arg1, u8 *anim) {
    *(u16 *)(anim + 0x8) = D_800E2348;
    *(s16 *)(anim + 0xA) = *(s16 *)(D_8009D254 + 0x2E);
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0x224;
    *(u16 *)(anim + 0xC) = D_800E234C;
}
