void Spu_RebaseStreamAddrs(unsigned char *ptr, int value, int count) {
    unsigned char *next = ptr + 4;
    int delta = value - *(int *)ptr;

    do {
        *(int *)ptr += delta;
        *(int *)next += delta;
        ptr += 0x40;
        next += 0x40;
        count--;
    } while (count != 0);
}
