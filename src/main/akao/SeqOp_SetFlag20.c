void SeqOp_SetFlag20(void *ptr) {
    *(int *)((char *)ptr + 0x38) |= 0x20;
}
