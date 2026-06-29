void SeqOp_StopPitchLFO(void *ptr) {
    *(short *)((char *)ptr + 0xE8) = 0;
    *(int *)((char *)ptr + 0x38) &= ~1;
    *(int *)((char *)ptr + 0xF4) |= 0x10;
}
