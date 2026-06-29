void mem2mem(int *dst, int *src, unsigned int count) {
    unsigned int i;
    int value;

    i = 0;
    if (count != 0) {
        do {
            value = *src;
            src++;
            i++;
            *dst = value;
            dst++;
        } while (i < count);
    }
}
