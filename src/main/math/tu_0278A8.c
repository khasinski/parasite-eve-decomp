

int Math_FixedDivide(int a, int b) {
    return (a / (b >> 8)) << 8;
}

int Math_FixedRoundToInt(int arg0) {
    register int bias asm("$1");
    int value;

    bias = 0x8000;
    asm volatile("add\t%0,%1,%2" : "=r"(value) : "r"(arg0), "r"(bias));
    return value >> 16;
}

int Math_FixedRoundToByte(int arg0) {
    register int bias asm("$1");
    int value;

    bias = 0x8000;
    asm volatile("add\t%0,%1,%2" : "=r"(value) : "r"(arg0), "r"(bias));
    return value >> 8;
}
