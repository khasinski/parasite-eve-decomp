typedef int s32;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3Fixed;

s32 func_800DFC80(Vec3Fixed *lhs, Vec3Fixed *rhs) {
    s32 dx = (lhs->x - rhs->x) >> 16;
    s32 dx_sq = dx * dx;
    s32 dz = (lhs->z - rhs->z) >> 16;
    s32 dz_sq = dz * dz;
    s32 horizontal;
    s32 dy;
    s32 rhs_y;
    s32 lhs_y;
    s32 distance_sq;
    s32 shift;
    s32 threshold;

    horizontal = 0;
    rhs_y = rhs->y;
    lhs_y = lhs->y;
    shift = 0x1E;
    dy = (lhs_y - rhs_y) >> 16;
    distance_sq = dx_sq + dz_sq;
    do {
        threshold = ((horizontal << 2) + 1) << shift;

        horizontal <<= 1;
        if (distance_sq >= threshold) {
            distance_sq -= threshold;
            horizontal |= 1;
        }
        shift -= 2;
    } while (shift >= 0);

    {
        s32 dy_sq;
        s32 horizontal_sq;
        s32 result;
        s32 shift2;
        s32 distance_sq2;

        dy_sq = dy * dy;
        horizontal_sq = horizontal * horizontal;
        distance_sq2 = dy_sq + horizontal_sq;
        result = 0;
        shift2 = 0x1E;
        do {
            threshold = ((result << 2) + 1) << shift2;

            result <<= 1;
            if (distance_sq2 >= threshold) {
                distance_sq2 -= threshold;
                result |= 1;
            }
            shift2 -= 2;
        } while (shift2 >= 0);

        return result;
    }
}
