void SeqOp_PushLoopPoint(void *ptr) {
    unsigned short index = (*(unsigned short *)((char *)ptr + 0xCE) + 1) & 3;

    *(volatile short *)((char *)ptr + 0xCE) = index;
    *(void **)((char *)ptr + 4 + index * 4) = *(void **)ptr;
    index = *(unsigned short *)((char *)ptr + 0xCE);
    *(short *)((char *)ptr + 0x62 + index * 2) = 0;
}
