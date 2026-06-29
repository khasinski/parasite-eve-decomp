void SeqOp_UpdateVolumeLFOTarget(void *ptr) {
    unsigned char *cursor = *(unsigned char **)ptr;

    *(unsigned char **)ptr = cursor + 1;
    *(short *)((char *)ptr + 0xA6) = *cursor << 8;
}
