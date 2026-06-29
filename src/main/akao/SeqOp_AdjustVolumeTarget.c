void SeqOp_AdjustVolumeTarget(void *ptr) {
    char *cursor = *(char **)ptr;
    int value;

    *(char **)ptr = cursor + 1;
    value = *(signed char *)cursor;
    if (value != 0) {
        value += *(short *)((char *)ptr + 0xD0);
        if (value <= 0) {
            value = 1;
        } else if (value >= 0x100) {
            value = 0xFF;
        }
    }

    *(short *)((char *)ptr + 0xD2) = value;
}
