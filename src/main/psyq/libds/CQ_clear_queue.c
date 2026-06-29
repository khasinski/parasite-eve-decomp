void CQ_clear_queue(void *arg0) {
    int i;
    char *ptr;
    char *bytes = arg0;
    int *words = arg0;

    words[0] = 0;
    i = 3;
    ptr = bytes + 3;
    bytes[4] = 0;
    for (; i >= 0; i--) {
        ptr[5] = 0;
        ptr--;
    }
    words[3] = 0;
    words[4] = 0;
    words[5] = 0;
}
