void SeqOp_ClearFlag20(void *ptr) {
    *(int *)((char *)ptr + 0x38) &= ~0x20;
}
