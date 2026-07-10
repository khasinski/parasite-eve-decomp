/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned int u32;

typedef struct MathU64 {
    u32 lo;
    u32 hi;
} MathU64;

MathU64 *Math_Add64(MathU64 *out, u32 a_lo, u32 a_hi, u32 b_lo, u32 b_hi) {
    register MathU64 *out_reg asm("$9");
    u32 parts[4];
    u32 low_sum;
    u32 high_sum;
    register u32 carry_mask asm("$5");

    out_reg = out;
    asm volatile("" : : "r"(out_reg));
    parts[1] = a_lo >> 16;
    parts[0] = a_lo & 0xFFFF;
    parts[3] = b_lo >> 16;
    parts[2] = b_lo & 0xFFFF;
    asm volatile("" : : "r"(parts) : "memory");
    low_sum = parts[0] + parts[2];
    parts[0] = low_sum;
    carry_mask = 0x10000;
    if (low_sum & carry_mask) {
        parts[1]++;
    }

    high_sum = parts[1] + parts[3];
    parts[1] = high_sum;
    if (high_sum & carry_mask) {
        a_hi++;
    }

    out_reg->lo = (high_sum << 16) | (low_sum & 0xFFFF);
    out_reg->hi = a_hi + b_hi;
    return out_reg;
}
