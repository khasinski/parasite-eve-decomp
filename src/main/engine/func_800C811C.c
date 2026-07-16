typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

extern char *D_8009D254;
extern u16 D_800E2348;
extern u16 D_800E234C;

int func_800C811C(void *arg0, void *arg1, u8 *anim) {
    char *entity_v1 = D_8009D254;
    int value;
    int z_v1;

    *(u16 *)(anim + 0x8) = D_800E2348;
    value = *(s16 *)(entity_v1 + 0x2E);
    *(u16 *)(anim + 0xA) = value;
    z_v1 = D_800E234C;
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0x224;
    *(u16 *)(anim + 0xC) = z_v1;
}
