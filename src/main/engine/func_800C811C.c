typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

extern char *D_8009D254;
extern u16 D_800E2348;
extern u16 D_800E234C;

int func_800C811C(void *arg0, void *arg1, u8 *anim) {
    register char *entity_v1 asm("$3") = D_8009D254;
    register int value_v0 asm("$2");
    register int z_v1 asm("$3");

    *(u16 *)(anim + 0x8) = D_800E2348;
    asm volatile(
        "lh %0,46(%1)\n\t"
        "nop\n\t"
        "sh %0,10(%2)"
        : "=r"(value_v0)
        : "r"(entity_v1), "r"(anim)
        : "memory");
    z_v1 = D_800E234C;
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0x224;
    *(u16 *)(anim + 0xC) = z_v1;
}
