void SeqOp_SetPitchBase(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;

    *(unsigned char **)ptr = cursor + 1;
    *(short *)((char *)ptr + 0xB4) = *cursor << 7;
}
