void *Widget_GetDataPtr(int *arg0) {
    char *base;
    register int offset asm("$3");

    if ((arg0[1] & 8) != 0) {
        offset = arg0[2];
        base = (char *)arg0 + 8;
        base += offset;
    } else {
        base = (char *)arg0 + 8;
    }
    return base + 4;
}
