int Math_FixedMul(int a, int b) {
    register int lo asm("$2");
    register int hi asm("$3");

    asm volatile(
        "mult %2,%3\n"
        "mflo %0\n"
        "mfhi %1"
        : "=r"(lo), "=r"(hi)
        : "r"(a), "r"(b));

    lo = (unsigned int)lo >> 16;
    hi <<= 16;
    return hi | lo;
}
