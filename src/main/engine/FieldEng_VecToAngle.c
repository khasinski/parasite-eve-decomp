typedef short s16;
typedef int s32;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3Fixed;

s32 Gte_Atan2(s32 x, s32 z);

s16 FieldEng_VecToAngle(Vec3Fixed *from, Vec3Fixed *to) {
    return Gte_Atan2(to->x - from->x, to->z - from->z);
}
