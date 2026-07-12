typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

extern char *volatile D_8009D254;
extern volatile u16 D_800E2360;
extern volatile u16 D_800E2364;

int func_800CAB88(void *arg0, void *arg1, u8 *anim) {
    u16 x = D_800E2360;
    char *data = D_8009D254;
    register s16 y asm("$2");
    u16 z;

    *(u16 *)(anim + 0x8) = x;
    asm volatile("lh $2,0x2e(%1)" : "=r"(y) : "r"(data));
    *(u16 *)(anim + 0xA) = y;
    z = D_800E2364;
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0x224;
    *(u16 *)(anim + 0xC) = z;
}
