void SeqOp_ClearFlag3(void *ptr) {
    *(int *)((char *)ptr + 0x38) &= ~8;
}
