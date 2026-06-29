void SeqOp_SetTrack34Mask(void *ptr);

void SeqOp_SetDurationBAAndMask(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;
    int value;

    *(unsigned char **)ptr = cursor + 1;
    value = *cursor;
    if (value != 0) {
        value++;
    } else {
        value = 0x101;
    }
    *(short *)((char *)ptr + 0xBA) = value;
    SeqOp_SetTrack34Mask(ptr);
}
