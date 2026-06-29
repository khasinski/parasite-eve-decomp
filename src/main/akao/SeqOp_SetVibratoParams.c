void SeqOp_SetVibratoParams(void *ptr) {
    unsigned char *cursor;
    unsigned char *next_cursor;
    int value;

    cursor = *(unsigned char **)ptr;
    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    *(short *)((char *)ptr + 0x7E) = value;
    if (value == 0) {
        *(short *)((char *)ptr + 0x7E) = 0x100;
    }

    next_cursor = *(unsigned char **)ptr;
    *(unsigned char **)ptr = next_cursor + 1;
    *(short *)((char *)ptr + 0xE4) = (signed char)*next_cursor;
}
