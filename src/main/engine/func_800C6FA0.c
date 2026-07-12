typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

void func_800C6FA0(char *data, u16 factor) {
    int i;
    u8 *entry = (u8 *)(data + *(u16 *)(data + 0x8));
    volatile u32 *scratch = (volatile u32 *)0x1F800000;

    for (i = 0; i < *(u16 *)(data + 0xA); i++, entry += 4) {
        u32 value;

        value = entry[0] * factor;
        scratch[12] = value;
        if (value > 0x7FFF) {
            scratch[12] = 0x7FFF;
        }
        value = scratch[12] >> 7;
        scratch[12] = value;
        entry[0] = value;

        value = entry[1] * factor;
        scratch[12] = value;
        if (value > 0x7FFF) {
            scratch[12] = 0x7FFF;
        }
        value = scratch[12] >> 7;
        scratch[12] = value;
        entry[1] = value;

        value = entry[2];
        asm volatile("nop");
        value *= factor;
        scratch[12] = value;
        if (value > 0x7FFF) {
            scratch[12] = 0x7FFF;
        }
        value = scratch[12] >> 7;
        scratch[12] = value;
        entry[2] = value;
    }
}
