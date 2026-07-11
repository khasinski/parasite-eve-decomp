typedef long long s64;

int Math_FixedMul(int a, int b) {
    return (int)(((s64)a * b) >> 16);
}
