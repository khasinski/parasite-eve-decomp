typedef int s32;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3Fixed;

s32 func_800DFC80(Vec3Fixed *lhs, Vec3Fixed *rhs) {
    register s32 dx asm("$2") = (lhs->x - rhs->x) >> 16;
    register s32 dx_sq asm("$8") = dx * dx;
    register s32 dz asm("$2") = (lhs->z - rhs->z) >> 16;
    register s32 dz_sq asm("$10") = dz * dz;
    register s32 horizontal asm("$6");
    register s32 dy asm("$7");
    register s32 rhs_y asm("$3");
    register s32 lhs_y asm("$2");
    register s32 distance_sq asm("$5");
    register s32 shift asm("$4");
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
        register s32 dy_sq asm("$2");
        register s32 horizontal_sq asm("$3");
        register s32 result asm("$4");
        register s32 shift2 asm("$5");
        register s32 distance_sq2 asm("$6");

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
