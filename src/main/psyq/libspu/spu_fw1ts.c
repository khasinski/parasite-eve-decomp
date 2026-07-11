/* MASPSX_FLAGS: --stack-return-delay --store-jump-delay */

void _spu_Fw1ts(void) {
    volatile int i;
    volatile int value;
    int current;

    value = 0xD;
    i = 0;
    while (i < 0x3C) {
        current = value;
        value = (((current << 1) + current) << 2) + current;
        i = i + 1;
    }
}
