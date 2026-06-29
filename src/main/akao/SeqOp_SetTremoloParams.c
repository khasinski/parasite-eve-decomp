void SeqOp_SetTremoloParams(void *ptr) {
    unsigned char *cursor;
    int value;

    cursor = *(unsigned char **)ptr;
    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    *(short *)((char *)ptr + 0x82) = value;
    if (value == 0) {
        *(short *)((char *)ptr + 0x82) = 0x100;
    }

    *(short *)((char *)ptr + 0xE6) = 0;
    *(short *)((char *)ptr + 0x80) = 0;
    *(short *)((char *)ptr + 0x84) = 1;
}
