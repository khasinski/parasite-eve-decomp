extern unsigned char D_800A5B70[];

int CardObj_GetChannelId(unsigned char *entry) {
    unsigned char *candidate;
    int index;
    int value;

    index = 0;
    value = 0x10;
    candidate = D_800A5B70;
    for (; index < 2; index++) {
        if (entry == candidate) {
            return value;
        }
        value += 0x10;
        do {
            candidate += 0xF0;
        } while (0);
    }

    return 0xFF;
}
