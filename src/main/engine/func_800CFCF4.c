typedef signed short s16;

int func_800CFCF4(int from, int to, s16 *distance) {
    register int direction asm("$7");
    register int delta asm("$3");
    register int direction16 asm("$2");
    int diff;

    from &= 0xFFF;
    to &= 0xFFF;
    direction = -1;
    if ((unsigned int)to < (unsigned int)from) {
        direction = 1;
    }

    diff = from - to;
    delta = diff;
    if (diff < 0) {
        delta = -delta;
    }

    if (delta >= 0x801) {
        direction16 = (s16)direction;
        direction = -direction16;
        delta = 0x1000 - delta;
    }

    if (distance != 0) {
        *distance = delta;
    }

    return (s16)direction;
}
