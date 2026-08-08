#include "common.h"
#include "pe1/gte.h"

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

extern s16 D_80096250[];

/*
 * The retail routine normalizes through the GTE: SQR obtains the squared
 * length, a reciprocal table supplies IR0, and GPF scales each component.
 */
static void Gte_NormalizeComponents(s32 x, s32 y, s32 z, s32 *out) {
    s32 square_x;
    s32 square_y;
    s32 square_z;
    s32 length_sq;
    s32 leading;
    s32 even_leading;
    s32 shift;
    s32 table_index;
    s32 scale;

    gte_ldir123(x, y, z);
    gte_sqr();
    gte_getmac123(square_x, square_y, square_z);

    length_sq = square_x + square_y + square_z;
    gte_ldlzcs(length_sq);
    gte_getlzcr(leading);

    even_leading = leading & -2;
    shift = (31 - even_leading) >> 1;
    if (even_leading >= 24) {
        table_index = length_sq << (even_leading - 24);
    } else {
        table_index = length_sq >> (24 - even_leading);
    }
    scale = D_80096250[table_index - 64];

    gte_ldir0(scale);
    gte_ldir123(x, y, z);
    gte_gpf();
    gte_getmac123(out[0], out[1], out[2]);
    out[0] >>= shift;
    out[1] >>= shift;
    out[2] >>= shift;
}

void Gte_NormalizeVecS32toS16(Vec3i *src, Vec3s *dst) {
    s32 normalized[3];

    Gte_NormalizeComponents(src->x, src->y, src->z, normalized);
    dst->x = normalized[0];
    dst->y = normalized[1];
    dst->z = normalized[2];
}

void Gte_NormalizeVec(Vec3i *src, Vec3i *dst) {
    Gte_NormalizeComponents(src->x, src->y, src->z, &dst->x);
}
