typedef unsigned char u8;

void func_800C608C(int scale, u8 *src, u8 *dst) {
    if (scale != 0x80) {
        int value;

        value = src[0] * scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        dst[0] = value >> 7;

        value = src[1] * scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        dst[1] = value >> 7;

        value = src[2] * scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        dst[2] = value >> 7;
    } else {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
    }
}
