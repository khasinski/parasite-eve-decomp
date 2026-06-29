void SeqOp_ClearFlag10(void *ptr) {
    *(int *)((char *)ptr + 0x38) &= ~0x10;
}
