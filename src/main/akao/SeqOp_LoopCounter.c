void SeqOp_LoopCounter(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;
    int value;
    unsigned short index;
    unsigned short counter;

    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    if (value == 0) {
        value = 0x100;
    }

    index = *(unsigned short *)((char *)ptr + 0xCE);
    counter = *(unsigned short *)((char *)ptr + 0x62 + index * 2) + 1;
    *(short *)((char *)ptr + 0x62 + index * 2) = counter;

    if (counter != value) {
        index = *(unsigned short *)((char *)ptr + 0xCE);
        *(void **)ptr = *(void **)((char *)ptr + 4 + index * 4);
    } else {
        index = *(unsigned short *)((char *)ptr + 0xCE);
        index = (index - 1) & 3;
        *(short *)((char *)ptr + 0xCE) = index;
    }
}
