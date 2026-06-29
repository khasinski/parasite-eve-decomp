int Math_FixedDivide(int a, int b) {
    return (a / (b >> 8)) << 8;
}
