typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u8 *D_800F34F4;
extern char *D_800E2248;

int func_800C2E08(void) {
    int i;
    int offset;
    register int result asm("$7");
    register u32 andMask asm("$9");
    register u32 xorMask asm("$8");
    u8 *entry;

    result = 0;
    i = 0;
    andMask = 0xFFFF0000;
    xorMask = 0x01000000;
    offset = 0;
    for (; i < 0x40; i++, offset += 6) {
        if (((s8 *)(offset + (int)D_800F34F4))[1] != 0) {
            register u32 check asm("$2");

            entry = (u8 *)((u16)i * 6 + (int)D_800F34F4);
            entry[1] = 0;
            D_800E2248[6]--;
            check = *(u32 *)(D_800E2248 + 4) & andMask;
            asm("xor %0,%0,%3\n\tsltiu %0,%0,1\n\tnegu %0,%0\n\tor %1,%1,%0"
                : "=r"(check), "=r"(result)
                : "0"(check), "r"(xorMask), "1"(result));
        }
    }

    return result;
}
