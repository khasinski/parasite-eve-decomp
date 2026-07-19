
typedef short s16;
typedef int s32;
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
