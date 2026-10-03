/* MASPSX_FLAGS: --expand-div */
#include "fx_common.h"
#include "pe1/gte_types.h"

/* Sample a closed path of short points: position holds the segment index in
 * its upper bits and an 8-bit fraction below.  Writes the interpolated point
 * and a yaw/bank pair derived from the two neighbouring segment headings,
 * and returns the path's point count. */
int func_8018F55C(int position, int index, void *table,
                  FxCommonMotionVec *out, void *extra)
{
    GteVector start;
    GteVector middle;
    GteVector end;
    GteVector toMiddle;
    GteVector toEnd;
    GteShortVector point;
    FxCommonOffsetByte *base;
    FxCommonOffsetByte *cursor;
    GteShortVector *points;
    s16 *angles;
    int fraction;
    int segment;
    int next;
    int after;
    int count;
    int bank;
    int span;
    s16 yaw;
    s16 nextYaw;

    base = table;
    cursor = base + index * sizeof(s32);
    points = (GteShortVector *)(base + *(s32 *)cursor);
    angles = extra;
    segment = (u32)position >> 8;
    count = ((volatile GteShortVector *)points)->pad;
    fraction = position & 0xFF;
    points++;
    span = count - 2;
    if (span < segment)
        D_8019BFCC = 1;
    segment %= span;
    if (segment < 0)
        segment += count - 2;
    next = (segment + 1) % span;
    after = (segment + 2) % span;

    point = points[segment];
    start.x = point.x;
    start.y = point.y;
    start.z = point.z;
    point = points[next];
    middle.x = point.x;
    middle.y = point.y;
    middle.z = point.z;
    point = points[after];
    end.x = point.x;
    end.y = point.y;
    end.z = point.z;

    toMiddle.x = middle.x - start.x;
    toMiddle.y = middle.y - start.y;
    toMiddle.z = middle.z - start.z;
    toEnd.x = end.x - middle.x;
    toEnd.y = end.y - middle.y;
    toEnd.z = end.z - middle.z;

    yaw = -Gte_Atan2(toMiddle.z, toMiddle.x);
    nextYaw = -Gte_Atan2(toEnd.z, toEnd.x);
    if (yaw - nextYaw > 0x800)
        nextYaw += 0x1000;
    if (nextYaw - yaw > 0x800)
        yaw += 0x1000;

    bank = (yaw - nextYaw) >> 3;
    angles[2] = bank;
    angles[0] = 0;
    angles[1] = yaw + (((nextYaw - yaw) * fraction) >> 8);
    if (bank > 0x80)
        angles[2] = 0x80;
    if (angles[2] < -0x80)
        angles[2] = -0x80;
    if (angles[2] >= -3 && angles[2] <= 3)
        angles[2] = 0;

    start.x <<= 8;
    start.y <<= 8;
    start.z <<= 8;
    middle.x <<= 8;
    middle.y <<= 8;
    middle.z <<= 8;
    start.x += toMiddle.x * fraction;
    start.y += toMiddle.y * fraction;
    start.z += toMiddle.z * fraction;
    start.x >>= 8;
    start.y >>= 8;
    start.z >>= 8;

    out->x = start.x;
    out->y = start.y;
    out->z = start.z;
    return count;
}
