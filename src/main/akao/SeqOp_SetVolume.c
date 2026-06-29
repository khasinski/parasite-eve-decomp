void SeqOp_SetVolume(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;
    int value;

    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    *(short *)((char *)ptr + 0xD2) = 0;
    *(short *)((char *)ptr + 0x58) = value;
    *(short *)((char *)ptr + 0x56) = value;
    *(short *)((char *)ptr + 0xD0) = value;
}
