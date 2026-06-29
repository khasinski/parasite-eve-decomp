void SeqOp_StopTremolo(void *ptr) {
    *(short *)((char *)ptr + 0x82) = 0;
}
