typedef short s16;
typedef int s32;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3Fixed;

s32 Gte_Atan2(s32 x, s32 z);

void func_800DFE94(Vec3Fixed *from, Vec3Fixed *to, s16 *out) {
    Vec3Fixed *from_reg = from;
    Vec3Fixed *to_reg = to;
    register s32 dx asm("$2") = (from_reg->x - to_reg->x) >> 16;
    register s32 dx_sq asm("$6") = dx * dx;
    register s32 dz asm("$2") = (from_reg->z - to_reg->z) >> 16;
    register s32 dz_sq asm("$3") = dz * dz;
    register s32 horizontal asm("$5");
    register s32 shift asm("$4");
    register s32 distance_sq asm("$6");
    s32 threshold;
    s32 angle;

    horizontal = 0;
    shift = 0x1E;
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

    angle = Gte_Atan2(from_reg->y - to_reg->y, horizontal << 16);
    out[0] = angle & 0xFFF;

    angle = Gte_Atan2(to_reg->x - from_reg->x, to_reg->z - from_reg->z);
    out[1] = angle & 0xFFF;
    out[2] = 0;
}
