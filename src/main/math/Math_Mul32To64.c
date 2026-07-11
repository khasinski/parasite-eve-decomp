typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct MathU64 {
    u32 lo;
    u32 hi;
} MathU64;

MathU64 *Math_Mul32To64(MathU64 *out, u32 lhs, u32 rhs) {
    u64 result;

    result = (u64)lhs * rhs;
    out->lo = result;
    out->hi = result >> 32;
    return out;
}
