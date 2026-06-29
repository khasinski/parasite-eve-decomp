typedef unsigned char u8;

int CardObj_CalcReadPayloadSize(void *arg0) {
    int first;
    int second;
    int base;
    int raw_first;
    int raw_second;

    raw_first = *(u8 *)((char *)arg0 + 0xE3);
    raw_second = *(u8 *)((char *)arg0 + 0xE9);
    base = *(int *)((char *)arg0 + 0xEC);

    first = raw_first + 1;
    asm volatile("" : "=r"(first) : "0"(first));
    first >>= 1;
    first <<= 2;

    second = (raw_second << 2) + raw_second;
    second = (second + 3) & 0xFFC;
    second += 4;

    first += second;
    return first + base;
}
