void Akao_SetNotePitch(void *ptr, int arg);

void Akao_InitVoiceState(void *ptr, void *script) {
    *(short *)((char *)ptr + 0x6C) = 0x6E00;
    *(void **)ptr = script;
    *(short *)((char *)ptr + 0xDE) = 0;
    *(short *)((char *)ptr + 0xE0) = 0;
    *(short *)((char *)ptr + 0x82) = 0;
    *(int *)((char *)ptr + 0x34) = 0;
    *(short *)((char *)ptr + 0xE4) = 0;
    *(short *)((char *)ptr + 0x7A) = 0;
    *(short *)((char *)ptr + 0xD2) = 0;
    *(short *)((char *)ptr + 0xD0) = 0;
    *(int *)((char *)ptr + 0x44) = 0x32000000;
    *(short *)((char *)ptr + 0x72) = 0;
    *(short *)((char *)ptr + 0xCE) = 0;
    *(int *)((char *)ptr + 0x38) = 0;
    *(short *)((char *)ptr + 0xEC) = 0;
    *(short *)((char *)ptr + 0x84) = 0;
    *(short *)((char *)ptr + 0xB4) = 0;
    *(short *)((char *)ptr + 0xA6) = 0;
    *(short *)((char *)ptr + 0x94) = 0;
    *(short *)((char *)ptr + 0xB6) = 0;
    *(short *)((char *)ptr + 0xA8) = 0;
    *(short *)((char *)ptr + 0x96) = 0;
    *(short *)((char *)ptr + 0xBC) = 0;
    *(short *)((char *)ptr + 0xBA) = 0;
    Akao_SetNotePitch(ptr, 0);
}

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

void Spu_RebaseStreamAddrs(unsigned char *ptr, int value, int count) {
    unsigned char *next = ptr + 4;
    int delta = value - *(int *)ptr;

    do {
        *(int *)ptr += delta;
        *(int *)next += delta;
        ptr += 0x40;
        next += 0x40;
        count--;
    } while (count != 0);
}
