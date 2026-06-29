void SeqOp_SetSustainFlag(void *ptr) {
    *(int *)((char *)ptr + 0x38) |= 0x100000;
}
