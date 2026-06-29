unsigned int Akao_ForEachVoiceMasked(unsigned char *entry, unsigned int mask) {
    unsigned int bit;
    unsigned int result;
    unsigned int index;

    index = 0;
    result = 0;
    for (bit = 1; index < 24; index++, entry += 0x11C) {
        if ((mask & (bit << index)) != 0) {
            unsigned int target = *(unsigned int *)(entry + 0xF0);

            if (target < 24) {
                result |= bit << target;
            }
        }
    }
    return result;
}
