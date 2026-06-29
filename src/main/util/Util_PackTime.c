typedef unsigned int u32;

void Util_PackTime(u32 *src, u32 *out) {
    register u32 value asm("$7");
    u32 magic;
    u32 mask;
    u32 word;
    register u32 tmp asm("$3");
    u32 last;
    u32 packed;

    value = src[1];
    magic = 0x88888889;
    value %= 60;
    mask = 0xFFFFF;
    word = *out;
    word &= 0xFFF00000;
    tmp = value * 7;
    tmp <<= 5;
    tmp += value;
    packed = ((tmp * 15) << 6) & mask;
    word |= packed;
    *out = word;

    value = src[1];
    value %= 60;
    word &= 0xFC0FFFFF;
    packed = ((value * 3600) & 0x3F) << 20;
    word |= packed;
    *out = word;

    last = src[1];
    last %= 60;
    word &= 0x03FFFFFF;
    packed = (last * 15) << 28;
    word |= packed;
    *out = word;
}
