int func_800DFC44(int value) {
    int result = 0;
    int shift = 0x1E;

    do {
        int threshold = ((result << 2) + 1) << shift;

        result <<= 1;
        if (value >= threshold) {
            value -= threshold;
            result |= 1;
        }
        shift -= 2;
    } while (shift >= 0);

    return result;
}


#include "common.h"
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


#include "common.h"
typedef struct
{
  s16 x;
  s16 y;
  s16 z;
} Vec3s;
s32 func_800DFD54(Vec3s *lhs, Vec3s *rhs)
{
  register s32 lhs_x = lhs->x;
  register s32 rhs_x = rhs->x;
  register s32 dx = lhs_x - rhs_x;
  register s32 dx_sq = dx * dx;
  register s32 lhs_z = lhs->z;
  register s32 rhs_z = rhs->z;
  register s32 dz = lhs_z - rhs_z;
  register s32 dz_sq = dz * dz;
  register s32 horizontal;
  register short rhs_y;
  register s32 lhs_y;
  register s32 dy;
  register s32 distance_sq;
  register s32 shift;
  s32 threshold;
  horizontal = 0;
  rhs_y = rhs->y;
  lhs_y = lhs->y;
  shift = 0x1E;
  dy = lhs_y - rhs_y;
  distance_sq = dx_sq + dz_sq;
  do
  {
    threshold = ((horizontal << 2) + 1) << shift;
    horizontal <<= 1;
    if (distance_sq >= threshold)
    {
      distance_sq -= threshold;
      horizontal |= 1;
    }
    shift -= 2;
  }
  while (shift >= 0);
  {
    register s32 dy_sq;
    register s32 horizontal_sq;
    register s32 result;
    register s32 shift2;
    register s32 distance_sq2;
    dy_sq = dy * dy;
    horizontal_sq = horizontal * horizontal;
    distance_sq2 = dy_sq + horizontal_sq;
    result = 0;
    shift2 = 0x1E;
    do
    {
      threshold = ((result << 2) + 1) << shift2;
      result <<= 1;
      if (distance_sq2 >= threshold)
      {
        distance_sq2 -= threshold;
        result |= 1;
      }
      shift2 -= 2;
    }
    while (shift2 >= 0);
    return result;
  }
}


#include "common.h"

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


#include "common.h"

s32 Gte_Atan2(s32 x, s32 z);

void func_800DFE94(Vec3Fixed *from, Vec3Fixed *to, s16 *out) {
    Vec3Fixed *from_reg = from;
    Vec3Fixed *to_reg = to;
    s32 dx = (from_reg->x - to_reg->x) >> 16;
    s32 dx_sq = dx * dx;
    s32 dz = (from_reg->z - to_reg->z) >> 16;
    s32 dz_sq = dz * dz;
    s32 horizontal;
    s32 shift;
    s32 distance_sq;
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


#include "common.h"

s32 Gte_Atan2(s32 x, s32 z);

s16 FieldEng_VecToAngle(Vec3Fixed *from, Vec3Fixed *to) {
    return Gte_Atan2(to->x - from->x, to->z - from->z);
}
