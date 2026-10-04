/* 24.8 fixed-point quotient: drops the divisor's fraction byte, then
 * restores the scale of the result. */
int Math_FixedDivide(int a, int b) {
    return (a / (b >> 8)) << 8;
}
