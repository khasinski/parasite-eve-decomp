void SeqOp_SetDuration(unsigned char *arg0) {
    unsigned char *ptr = *(unsigned char **)arg0;
    int value;

    *(unsigned char **)arg0 = ptr + 1;
    value = ptr[0];
    if (value == 0) {
        value = 0x101;
    } else {
        value++;
    }
    *(unsigned short *)(arg0 + 0xBA) = value;
}
