typedef unsigned char u8;
typedef unsigned short u16;

void func_800C7098(char *data, u8 r, u8 g, u8 b) {
    int i;
    u8 *dst = (u8 *)(data + *(u16 *)(data + 0x8));

    for (i = 0; i < *(u16 *)(data + 0xA); i++, dst += 4) {
        dst[0] = r;
        dst[1] = g;
        dst[2] = b;
    }
}
