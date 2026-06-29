void SeqOp_SetReturnPoint(void *ptr) {
    unsigned char **stream;
    unsigned char *cursor;
    unsigned char *next_cursor;
    int value;

    stream = (unsigned char **)ptr;
    cursor = *stream;
    value = *(*stream)++;
    next_cursor = cursor + 2;
    *stream = next_cursor;
    value |= cursor[1] << 8;

    *(short *)((char *)ptr + 0x5A) = 0xFF;
    *(short *)((char *)ptr + 0xE2) = 0;
    *(unsigned int *)((char *)ptr + 0x38) =
        *(unsigned int *)((char *)ptr + 0x38) | 0x1000;
    *(unsigned char **)((char *)ptr + 0x18) = next_cursor + (short)value;
}
