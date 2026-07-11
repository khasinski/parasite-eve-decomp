typedef signed short s16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;

typedef struct Vec3i {
    s32 x;
    s32 y;
    s32 z;
} Vec3i;

typedef struct Vec3s {
    s16 x;
    s16 y;
    s16 z;
} Vec3s;

int Gte_ISqrt(int value);

static u64 Gte_DivU64ByU32(u64 numerator, u32 denominator) {
    u64 quotient;
    u32 remainder;
    s32 i;

    quotient = 0;
    remainder = 0;
    for (i = 0; i < 64; i++) {
        remainder <<= 1;
        remainder |= numerator >> 63;
        numerator <<= 1;
        quotient <<= 1;

        if (remainder >= denominator) {
            remainder -= denominator;
            quotient |= 1;
        }
    }

    return quotient;
}

static s32 Gte_NormalizeComponent(s32 value, s32 length) {
    u64 magnitude;
    u64 quotient;

    if (length == 0) {
        return 0;
    }

    if (value < 0) {
        magnitude = (u64)-((s64)value);
    } else {
        magnitude = value;
    }

    quotient = Gte_DivU64ByU32(magnitude << 12, length);
    if (quotient > 0x7FFFFFFF) {
        quotient = 0x7FFFFFFF;
    }

    if (value < 0) {
        return -(s32)quotient;
    }
    return (s32)quotient;
}

void Gte_NormalizeVec(Vec3i *src, Vec3i *dst) {
    s32 x;
    s32 y;
    s32 z;
    s32 length;

    x = src->x;
    y = src->y;
    z = src->z;
    length = Gte_ISqrt((x * x) + (y * y) + (z * z));

    dst->x = Gte_NormalizeComponent(x, length);
    dst->y = Gte_NormalizeComponent(y, length);
    dst->z = Gte_NormalizeComponent(z, length);
}

void Gte_NormalizeVecS32toS16(Vec3i *src, Vec3s *dst) {
    Vec3i normalized;

    Gte_NormalizeVec(src, &normalized);

    dst->x = normalized.x;
    dst->y = normalized.y;
    dst->z = normalized.z;
}
