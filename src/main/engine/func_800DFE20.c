typedef int s32;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3Fixed;

s32 func_800DFE20(Vec3Fixed *lhs, Vec3Fixed *rhs) {
    s32 dx = (lhs->x - rhs->x) >> 16;
    s32 dx_sq = dx * dx;
    s32 dz = (lhs->z - rhs->z) >> 16;
    s32 dz_sq = dz * dz;
    s32 result;
    s32 shift;
    s32 distance_sq;
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
