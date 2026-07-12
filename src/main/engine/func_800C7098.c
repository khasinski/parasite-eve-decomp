typedef unsigned char u8;
typedef unsigned short u16;

void func_800C7098(char *data, u8 r, u8 g, u8 b) {
    register int i asm("$9") = 0;
    register int count asm("$3");
    u8 *dst = (u8 *)(data + *(u16 *)(data + 0x8));
    /* Keeps the empty 8-byte frame emitted by the original. */
    volatile int pad;

    count = *(u16 *)(data + 0xA);
    if (count > 0) {
        do {
            i++;
            dst[0] = r;
            dst[1] = g;
            dst[2] = b;
            dst += 4;
        } while (i < *(u16 *)(data + 0xA));
    }
}
