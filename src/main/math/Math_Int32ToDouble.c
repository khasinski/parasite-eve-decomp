typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct MathU64 {
    u32 lo;
    u32 hi;
} MathU64;

MathU64 *Math_Shift64(MathU64 *out, int direction, u32 lo, u32 hi, int amount);

u64 Math_Int32ToDouble(int value) {
    MathU64 parts;
    u32 sign;
    u32 exponent;
    u32 magnitude;
    u32 hi;
    u32 lo;

    sign = 0;
    exponent = 0x41D;
    if (value == 0) {
        lo = 0;
        hi = 0;
    } else {
        if (value < 0) {
            sign = 0x80000000;
            value = -value;
        }

        magnitude = value;
        if (magnitude <= 0x00FFFFFF) {
            do {
                magnitude <<= 4;
                exponent -= 4;
            } while (magnitude <= 0x00FFFFFF);
        }

        if (magnitude <= 0x3FFFFFFF) {
            do {
                magnitude <<= 1;
                exponent--;
            } while (magnitude <= 0x3FFFFFFF);
        }

        parts.lo = 0;
        parts.hi = magnitude;
        Math_Shift64(&parts, 1, parts.lo, parts.hi, 10);
        hi = (parts.hi & 0xFFEFFFFF) | sign | (exponent << 20);
        lo = parts.lo;
        parts.hi = parts.hi & 0xFFEFFFFF;
    }

    return ((u64)hi << 32) | lo;
}
