void SeqOp_ResetFlag4State(void *ptr) {
    *(short *)((char *)ptr + 0xEC) = 0;
    *(int *)((char *)ptr + 0x38) &= ~4;
    *(int *)((char *)ptr + 0xF4) |= 3;
}
