typedef int s32;
typedef unsigned short u16;

extern char *func_800C2B50(void);

void func_8018FE9C(s32 arg0, s32 arg1, char *arg2) {
    register char *src asm("$5");
    register char *dst asm("$3");
    register unsigned int i asm("$4");

    src = func_800C2B50();
    i = 0;
    dst = arg2;
    do {
        *(u16 *)(dst + 0) = *(s32 *)(src + 0x18);
        *(u16 *)(dst + 2) = *(s32 *)(src + 0x1C);
        *(u16 *)(dst + 4) = *(s32 *)(src + 0x20);
        i++;
        dst += 8;
    } while (i < 0x10);

    *(u16 *)(arg2 + 0x82) = 0x80;
    *(u16 *)(arg2 + 0x80) = 0x800;
}
