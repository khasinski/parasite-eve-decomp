typedef int s32;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3Fixed;

s32 func_800DFE20(Vec3Fixed *lhs, Vec3Fixed *rhs) {
    register s32 dx asm("$2") = (lhs->x - rhs->x) >> 16;
    register s32 dx_sq asm("$6") = dx * dx;
    register s32 dz asm("$2") = (lhs->z - rhs->z) >> 16;
    register s32 dz_sq asm("$3") = dz * dz;
    register s32 result asm("$4");
    register s32 shift asm("$5");
    register s32 distance_sq asm("$6");
    s32 threshold;

    result = 0;
    shift = 0x1E;
    distance_sq = dx_sq + dz_sq;
    do {
        threshold = ((result << 2) + 1) << shift;

        result <<= 1;
        if (distance_sq >= threshold) {
            distance_sq -= threshold;
            result |= 1;
        }
        shift -= 2;
    } while (shift >= 0);

    return result;
}
