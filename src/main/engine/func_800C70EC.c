typedef unsigned char u8;
typedef unsigned short u16;

void func_800C70EC(char *data, int rDelta, int gDelta, int bDelta) {
    int i;
    u8 *entry = (u8 *)(data + *(u16 *)(data + 0x8));

    for (i = 0; i < *(u16 *)(data + 0xA); i++, entry += 4) {
        int value;

        value = entry[0] + rDelta;
        if (value >= 0x100) {
            value = 0xFF;
        }
        if (value < 0) {
            value = 0;
        }
        entry[0] = value;

        value = entry[1] + gDelta;
        if (value >= 0x100) {
            value = 0xFF;
        }
        if (value < 0) {
            value = 0;
        }
        entry[1] = value;

        value = entry[2] + bDelta;
        if (value >= 0x100) {
            value = 0xFF;
        }
        if (value < 0) {
            value = 0;
        }
        entry[2] = value;
    }
}
