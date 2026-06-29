void SeqOp_SetFlag4(void *ptr) {
    *(int *)((char *)ptr + 0x38) |= 0x10;
}
