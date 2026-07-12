typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

void func_800C6FA0(char *data, u16 factor) {
    int i;
    u8 *entry = (u8 *)(data + *(u16 *)(data + 0x8));

    for (i = 0; i < *(u16 *)(data + 0xA); i++, entry += 4) {
        u32 value;

        value = entry[0] * factor;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        entry[0] = value >> 7;

        value = entry[1] * factor;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        entry[1] = value >> 7;

        value = entry[2] * factor;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        entry[2] = value >> 7;
    }
}
