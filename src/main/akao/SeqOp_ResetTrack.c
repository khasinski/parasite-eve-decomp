void SeqOp_ResetTrack(void *ptr) {
    *(short *)((char *)ptr + 0xEA) = 0;
    *(int *)((char *)ptr + 0x38) &= ~2;
    *(int *)((char *)ptr + 0xF4) |= 3;
}
