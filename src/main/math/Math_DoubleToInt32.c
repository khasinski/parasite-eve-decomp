typedef unsigned int u32;

typedef struct MathU64 {
    u32 lo;
    u32 hi;
} MathU64;

void Evt_Deliver(int type, int code);
MathU64 *Math_Add64WithShift(MathU64 *out, int direction, u32 lo, u32 hi, int amount);

int Math_DoubleToInt32(u32 lo, u32 hi) {
    MathU64 parts;
    u32 exponent;
    int shift;
    int value;

    if (((hi & 0x7FFFFFFF) == 0) && (lo == 0)) {
        return hi & 0x80000000;
    }

    exponent = (hi >> 20) & 0x7FF;
    shift = exponent - 0x41D;
    if (shift > 0) {
        Evt_Deliver(0x22, 0x11);
        if ((int)hi >= 0) {
            return 0x7FFFFFFF;
        }
        return 0x80000000;
    }

    parts.lo = lo;
    parts.hi = (hi & 0xFFFFF) | 0x100000;
    Math_Add64WithShift(&parts, 0, parts.lo, parts.hi, 10);

    if ((u32)(exponent - 0x3FE) >= 0x20 || parts.hi == 0) {
        return 0;
    }

    value = (int)parts.hi >> -shift;
    if ((int)hi < 0) {
        value = -value;
    }
    return value;
}
