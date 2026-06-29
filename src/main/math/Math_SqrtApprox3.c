/* CC1_FLAGS: -G8 -g3 */

unsigned int Math_SqrtApprox3(int x, int y, int z) {
    register unsigned int sum0 asm("$2");
    unsigned int sum1;
    register unsigned int sum2 asm("$6");
    register unsigned int sum3 asm("$7");

    if (x < 0) {
        x = -x;
    }
    if (y < 0) {
        y = -y;
    }
    if (z < 0) {
        z = -z;
    }

    {
        register unsigned int xlo asm("$7");
        register unsigned int xhi asm("$8");
        register unsigned int ylo asm("$9");
        register unsigned int yhi asm("$10");
        register unsigned int zlo asm("$4");
        unsigned int zhi;

        asm volatile(
            "mult\t%2,%2\n\t"
            "mflo\t%0\n\t"
            "mfhi\t%1\n\t"
            "nop\n\t"
            "nop\n"
            : "=r"(xlo), "=r"(xhi)
            : "r"(x));
        asm volatile(
            "mult\t%2,%2\n\t"
            "mflo\t%0\n\t"
            "mfhi\t%1\n\t"
            "nop\n\t"
            "nop\n"
            : "=r"(ylo), "=r"(yhi)
            : "r"(y));
        asm volatile(
            "mult\t%2,%2\n\t"
            "mflo\t%0\n\t"
            "mfhi\t%1\n"
            : "=r"(zlo), "=r"(zhi)
            : "r"(z));

        sum0 = xlo + ylo;
        sum1 = sum0 < xlo;
        sum1 += xhi;
        sum1 += yhi;
        sum2 = sum0 + zlo;
        sum3 = sum2 < sum0;
        sum3 += sum1;
        sum3 += zhi;
    }

    sum2 >>= 20;
    sum3 <<= 12;
    return sum3 | sum2;
}
