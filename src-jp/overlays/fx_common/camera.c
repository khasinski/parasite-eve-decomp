/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/cdrom.h"
#include "pe1/game_state.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/script_queue.h"

#include "camera_externs.h"
#include "fx_common.h"
#include "fx_common_motion.h"
#include "fx_common_setup.h"

typedef struct FxAssetSource {
    u8 reserved[0x100];
    u8 *data;
} FxAssetSource;

/* Aim the effect camera from its position at its target: build the view
 * matrix, the look-at matrix and the billboard rotation used by sprites. */
void FxCommon_UpdateCameraMatrices(void)
{
    GteVector offset;
    GteVector unit;
    RoomSpriteMatrix view;
    RoomSpriteMatrix rotation;
    GteShortVector target;
    GteShortVector eye;
    GteVector up;

    view = g_FxCommonIdentityMatrix;
    rotation = g_FxCommonIdentityMatrix;
    offset.x = g_FxCommonCameraTarget.x - g_FxCommonCameraPosition.x;
    offset.y = g_FxCommonCameraTarget.y - g_FxCommonCameraPosition.y;
    offset.z = g_FxCommonCameraTarget.z - g_FxCommonCameraPosition.z;
    VectorNormal(&offset, &unit);
    view.t[0] = -g_FxCommonCameraPosition.x;
    view.t[1] = -g_FxCommonCameraPosition.y;
    view.t[2] = -g_FxCommonCameraPosition.z;
    g_FxCommonCameraAngles->x = ratan2(unit.y, SquareRoot0(unit.x * unit.x + unit.z * unit.z));
    g_FxCommonCameraAngles->y = ratan2(unit.z, unit.x) - 0x400;
    g_FxCommonCameraAngles->z = 0;
    RotMatrix((RoomFxSeed8 *)g_FxCommonCameraAngles, &rotation);
    CompMatrix(&rotation, &view, &g_FxCommonViewMatrix);

    up.x = 0;
    up.y = -20000;
    up.z = 0;
    target.x = g_FxCommonCameraTarget.x;
    target.y = g_FxCommonCameraTarget.y;
    target.z = g_FxCommonCameraTarget.z;
    eye.x = g_FxCommonCameraPosition.x;
    eye.y = g_FxCommonCameraPosition.y;
    eye.z = g_FxCommonCameraPosition.z;
    FxCommon_BuildLookAtMatrix((GteMatrix *)&g_FxCommonViewMatrix, &eye, &target, &up);

    g_FxCommonCameraAngles->x = -ratan2(SquareRoot0(offset.x * offset.x + offset.z * offset.z),
                              -offset.y) + 0x400;
    g_FxCommonCameraAngles->y = -ratan2(offset.z, offset.x) + 0x400;
    g_FxCommonCameraAngles->z = 0;
    RotMatrixZYX(g_FxCommonCameraAngles, &g_FxCommonCameraMatrix);
    g_FxCommonCameraMatrix.t[0] = g_FxCommonCameraPosition.x;
    g_FxCommonCameraMatrix.t[1] = g_FxCommonCameraPosition.y;
    g_FxCommonCameraMatrix.t[2] = g_FxCommonCameraPosition.z;
    SetTransMatrix(&g_FxCommonViewMatrix);
    SetRotMatrix(&g_FxCommonViewMatrix);
}

/* Build a camera matrix looking from eye toward target with the given up
 * vector: the rows are the normalised side, up and forward axes, and the
 * translation is the rotated eye position negated. */
void FxCommon_BuildLookAtMatrix(GteMatrix *out, GteShortVector *eye,
                   GteShortVector *target, GteVector *up)
{
    GteVector work;
    GteVector side;
    GteVector upAxis;
    GteVector forward;

    work.x = target->x - eye->x;
    work.y = target->y - eye->y;
    work.z = target->z - eye->z;
    VectorNormal(&work, &forward);
    if (forward.z == up->z)
        forward.z++;

    gte_ldopv1_psyq(&forward);
    gte_ldopv2(up);
    gte_op12_psyq();
    gte_stmac(&work);
    VectorNormal(&work, &side);

    gte_ldopv1_psyq(&forward);
    gte_ldopv2(&side);
    gte_op12_psyq();
    gte_stmac(&work);
    VectorNormal(&work, &upAxis);

    out->m[0][0] = side.x;
    out->m[0][1] = side.y;
    out->m[0][2] = side.z;
    out->m[1][0] = upAxis.x;
    out->m[1][1] = upAxis.y;
    out->m[1][2] = upAxis.z;
    out->m[2][0] = forward.x;
    out->m[2][1] = forward.y;
    out->m[2][2] = forward.z;

    gte_ldrotmatrix(out);
    gte_ldv0(eye);
    gte_mvmva();
    gte_stmac(out->t);
    out->t[0] = -out->t[0];
    out->t[1] = -out->t[1];
    out->t[2] = -out->t[2];
}

/* Sample a closed path of short points: position holds the segment index in
 * its upper bits and an 8-bit fraction below.  Writes the interpolated point
 * and a yaw/bank pair derived from the two neighbouring segment headings,
 * and returns the path's point count. */
int FxCommon_SamplePath(u32 position, int index, void *table,
                  FxCommonVec3 *out, GteShortVector *extra)
{
    GteVector start;
    GteVector middle;
    GteVector end;
    GteVector toMiddle;
    GteVector toEnd;
    FxCommonPathPoint point;
    FxCommonOffsetByte *base;
    FxCommonOffsetByte *cursor;
    FxCommonPathPoint *points;
    GteShortVector *angles;
    int fraction;
    int segment;
    int next;
    int after;
    s16 count;
    int bank;
    int delta;
    int span;
    s16 yaw;
    s16 nextYaw;

    base = table;
    cursor = base + index * sizeof(s32);
    points = (FxCommonPathPoint *)(base + *(s32 *)cursor);
    angles = extra;
    segment = position >> 8;
    count = points->count;
    fraction = position & 0xFF;
    points++;
    span = count - 2;
    if (span < segment)
        g_FxCommonPathWrapped = 1;
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

    yaw = -ratan2(toMiddle.z, toMiddle.x);
    nextYaw = -ratan2(toEnd.z, toEnd.x);
    if (yaw - nextYaw > 0x800)
        nextYaw += 0x1000;
    if (nextYaw - yaw > 0x800)
        yaw += 0x1000;

    delta = nextYaw - yaw;
    delta *= fraction;
    bank = (yaw - nextYaw) >> 3;
    angles->z = bank;
    angles->x = 0;
    angles->y = yaw + (delta >> 8);
    if (bank > 0x80)
        angles->z = 0x80;
    if (angles->z < -0x80)
        angles->z = -0x80;
    if (angles->z >= -3 && angles->z <= 3)
        angles->z = 0;

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

/* Rotate the four frustum corners by the camera matrix and rebuild the
 * four side planes through the eye: normal, distance, the signed distance
 * of the next corner and the normal length. */
void FxCommon_UpdateFrustumPlanes(FxCommonVec3 *eye)
{
    GteVector corner0;
    GteVector corner1;
    GteVector corner2;
    GteVector corner3;
    GteVector a;
    GteVector b;
    s32 flag;

    PushMatrix();
    SetTransMatrix(&g_FxCommonCameraMatrix);
    SetRotMatrix(&g_FxCommonCameraMatrix);
    RotTrans(&g_FxCommonFrustumCorner0, &corner0, &flag);
    RotTrans(&g_FxCommonFrustumCorner1, &corner1, &flag);
    RotTrans(&g_FxCommonFrustumCorner2, &corner2, &flag);
    RotTrans(&g_FxCommonFrustumCorner3, &corner3, &flag);

    a.x = corner2.x - eye->x;
    a.y = corner2.y - eye->y;
    a.z = corner2.z - eye->z;
    b.x = corner0.x - eye->x;
    b.y = corner0.y - eye->y;
    b.z = corner0.z - eye->z;
    OuterProduct0(&a, &b, &g_FxCommonFrustumNormal0);

    a.x = corner3.x - eye->x;
    a.y = corner3.y - eye->y;
    a.z = corner3.z - eye->z;
    b.x = corner1.x - eye->x;
    b.y = corner1.y - eye->y;
    b.z = corner1.z - eye->z;
    OuterProduct0(&a, &b, &g_FxCommonFrustumNormal1);

    a.x = corner3.x - eye->x;
    a.y = corner3.y - eye->y;
    a.z = corner3.z - eye->z;
    b.x = corner2.x - eye->x;
    b.y = corner2.y - eye->y;
    b.z = corner2.z - eye->z;
    OuterProduct0(&a, &b, &g_FxCommonFrustumNormal2);

    a.x = corner1.x - eye->x;
    a.y = corner1.y - eye->y;
    a.z = corner1.z - eye->z;
    b.x = corner0.x - eye->x;
    b.y = corner0.y - eye->y;
    b.z = corner0.z - eye->z;
    OuterProduct0(&a, &b, &g_FxCommonFrustumNormal3);

    g_FxCommonFrustumDistance0 = -(g_FxCommonFrustumNormal0.x * corner2.x) - g_FxCommonFrustumNormal0.y * corner2.y
               - g_FxCommonFrustumNormal0.z * corner2.z;
    g_FxCommonFrustumDistance1 = -(g_FxCommonFrustumNormal1.x * corner3.x) - g_FxCommonFrustumNormal1.y * corner3.y
               - g_FxCommonFrustumNormal1.z * corner3.z;
    g_FxCommonFrustumDistance2 = -(g_FxCommonFrustumNormal2.x * corner2.x) - g_FxCommonFrustumNormal2.y * corner2.y
               - g_FxCommonFrustumNormal2.z * corner2.z;
    g_FxCommonFrustumDistance3 = -(g_FxCommonFrustumNormal3.x * corner1.x) - g_FxCommonFrustumNormal3.y * corner1.y
               - g_FxCommonFrustumNormal3.z * corner1.z;
    g_FxCommonFrustumSide0 = g_FxCommonFrustumNormal0.x * corner1.x + g_FxCommonFrustumNormal0.y * corner1.y
               + g_FxCommonFrustumNormal0.z * corner1.z + g_FxCommonFrustumDistance0;
    g_FxCommonFrustumSide1 = g_FxCommonFrustumNormal1.x * corner0.x + g_FxCommonFrustumNormal1.y * corner0.y
               + g_FxCommonFrustumNormal1.z * corner0.z + g_FxCommonFrustumDistance1;
    g_FxCommonFrustumSide2 = g_FxCommonFrustumNormal2.x * corner1.x + g_FxCommonFrustumNormal2.y * corner1.y
               + g_FxCommonFrustumNormal2.z * corner1.z + g_FxCommonFrustumDistance2;
    g_FxCommonFrustumSide3 = g_FxCommonFrustumNormal3.x * corner2.x + g_FxCommonFrustumNormal3.y * corner2.y
               + g_FxCommonFrustumNormal3.z * corner2.z + g_FxCommonFrustumDistance3;

    g_FxCommonFrustumLength0 = SquareRoot0(g_FxCommonFrustumNormal0.x * g_FxCommonFrustumNormal0.x
                               + g_FxCommonFrustumNormal0.y * g_FxCommonFrustumNormal0.y
                               + g_FxCommonFrustumNormal0.z * g_FxCommonFrustumNormal0.z);
    g_FxCommonFrustumLength1 = SquareRoot0(g_FxCommonFrustumNormal1.x * g_FxCommonFrustumNormal1.x
                               + g_FxCommonFrustumNormal1.y * g_FxCommonFrustumNormal1.y
                               + g_FxCommonFrustumNormal1.z * g_FxCommonFrustumNormal1.z);
    g_FxCommonFrustumLength2 = SquareRoot0(g_FxCommonFrustumNormal2.x * g_FxCommonFrustumNormal2.x
                               + g_FxCommonFrustumNormal2.y * g_FxCommonFrustumNormal2.y
                               + g_FxCommonFrustumNormal2.z * g_FxCommonFrustumNormal2.z);
    g_FxCommonFrustumLength3 = SquareRoot0(g_FxCommonFrustumNormal3.x * g_FxCommonFrustumNormal3.x
                               + g_FxCommonFrustumNormal3.y * g_FxCommonFrustumNormal3.y
                               + g_FxCommonFrustumNormal3.z * g_FxCommonFrustumNormal3.z);
    PopMatrix();
}

int FxCommon_IsPointInView(int *point)
{
    int first, second;
    int secondVisible;
    int firstVisible = 0;

    first = g_FxCommonFrustumNormal0.x * point[0] + g_FxCommonFrustumNormal0.y * point[1]
          + g_FxCommonFrustumNormal0.z * point[2] + g_FxCommonFrustumDistance0;
    second = g_FxCommonFrustumNormal1.x * point[0] + g_FxCommonFrustumNormal1.y * point[1]
           + g_FxCommonFrustumNormal1.z * point[2] + g_FxCommonFrustumDistance1;

    secondVisible = 0;
    if (first > 0 && g_FxCommonFrustumSide0 > 0)
        firstVisible = 1;
    if (first < 0 && g_FxCommonFrustumSide0 < 0)
        firstVisible = 1;
    if (second > 0 && g_FxCommonFrustumSide1 > 0)
        secondVisible = 1;
    if (second < 0 && g_FxCommonFrustumSide1 < 0)
        secondVisible = 1;

    return firstVisible & secondVisible;
}

int FxCommon_IsShortPointInView(short *point)
{
    int first, second;
    int secondVisible;
    int firstVisible = 0;

    first = g_FxCommonFrustumNormal0.x * point[0] + g_FxCommonFrustumNormal0.y * point[1]
          + g_FxCommonFrustumNormal0.z * point[2] + g_FxCommonFrustumDistance0;
    second = g_FxCommonFrustumNormal1.x * point[0] + g_FxCommonFrustumNormal1.y * point[1]
           + g_FxCommonFrustumNormal1.z * point[2] + g_FxCommonFrustumDistance1;

    secondVisible = 0;
    if (first > 0 && g_FxCommonFrustumSide0 > 0)
        firstVisible = 1;
    if (first < 0 && g_FxCommonFrustumSide0 < 0)
        firstVisible = 1;
    if (second > 0 && g_FxCommonFrustumSide1 > 0)
        secondVisible = 1;
    if (second < 0 && g_FxCommonFrustumSide1 < 0)
        secondVisible = 1;

    return firstVisible & secondVisible;
}

int FxCommon_CheckBoundsWithMargin(int *point, int margin)
{
    int first;
    int second;
    int distance;
    int limit;
    int secondVisible = 0;
    int firstVisible = 0;
    int threshold = margin;
    first = g_FxCommonFrustumNormal0.x * point[0] + g_FxCommonFrustumNormal0.y * point[1]
          + g_FxCommonFrustumNormal0.z * point[2] + g_FxCommonFrustumDistance0;
    second = g_FxCommonFrustumNormal1.x * point[0] + g_FxCommonFrustumNormal1.y * point[1]
           + g_FxCommonFrustumNormal1.z * point[2] + g_FxCommonFrustumDistance1;
    if (g_FxCommonFrustumSide0 > 0 && first >= 0)
    {
      firstVisible = 1;
    }
    if (g_FxCommonFrustumSide0 < 0 && first <= 0)
    {
      firstVisible = 1;
    }
    if (g_FxCommonFrustumSide1 > 0 && second >= 0)
    {
      secondVisible = 1;
    }
    if (g_FxCommonFrustumSide1 < 0 && second <= 0)
    {
      secondVisible = 1;
    }
    limit = threshold * 8;
    threshold = limit + 30;
    if (!firstVisible)
    {
      if (first >= 0)
      {
        distance = first / g_FxCommonFrustumLength0;
      }
      else
      {
        distance = (-first) / g_FxCommonFrustumLength0;
      }
      limit = (short)threshold;
      if (distance < limit)
      {
        firstVisible = 1;
      }
    }
    if (!secondVisible)
    {
      if (second >= 0)
      {
        distance = second / g_FxCommonFrustumLength1;
      }
      else
      {
        distance = (-second) / g_FxCommonFrustumLength1;
      }
      limit = (short)threshold;
      if (distance < limit)
      {
        secondVisible = 1;
      }
    }
    return firstVisible & secondVisible;
}

int FxCommon_CheckFourBoundsWithMargin(int *point, int margin) {
    int first, second, third, fourth, distance;
    int limit;
    int fourthVisible = 0, thirdVisible = 0, secondVisible = 0, firstVisible = 0;
    int threshold = margin;
    first = g_FxCommonFrustumNormal0.x * point[0] + g_FxCommonFrustumNormal0.y * point[1]
          + g_FxCommonFrustumNormal0.z * point[2] + g_FxCommonFrustumDistance0;
    second = g_FxCommonFrustumNormal1.x * point[0] + g_FxCommonFrustumNormal1.y * point[1]
          + g_FxCommonFrustumNormal1.z * point[2] + g_FxCommonFrustumDistance1;
    third = g_FxCommonFrustumNormal2.x * point[0] + g_FxCommonFrustumNormal2.y * point[1]
          + g_FxCommonFrustumNormal2.z * point[2] + g_FxCommonFrustumDistance2;
    fourth = g_FxCommonFrustumNormal3.x * point[0] + g_FxCommonFrustumNormal3.y * point[1]
          + g_FxCommonFrustumNormal3.z * point[2] + g_FxCommonFrustumDistance3;
    if (g_FxCommonFrustumSide0 > 0 && first >= 0)
        firstVisible = 1;
    if (g_FxCommonFrustumSide0 < 0 && first <= 0)
        firstVisible = 1;
    if (g_FxCommonFrustumSide1 > 0 && second >= 0)
        secondVisible = 1;
    if (g_FxCommonFrustumSide1 < 0 && second <= 0)
        secondVisible = 1;
    if (g_FxCommonFrustumSide2 > 0 && third >= 0)
        thirdVisible = 1;
    if (g_FxCommonFrustumSide2 < 0 && third <= 0)
        thirdVisible = 1;
    if (g_FxCommonFrustumSide3 > 0 && fourth >= 0)
        fourthVisible = 1;
    if (g_FxCommonFrustumSide3 < 0 && fourth <= 0)
        fourthVisible = 1;
    limit = threshold * 8;
    threshold = limit + 30;
    if (!firstVisible) {
        if (first >= 0)
            distance = first / g_FxCommonFrustumLength0;
        else
            distance = -first / g_FxCommonFrustumLength0;
        limit = (short)threshold;
        if (distance < limit)
            firstVisible = 1;
    }
    if (!secondVisible) {
        if (second >= 0)
            distance = second / g_FxCommonFrustumLength1;
        else
            distance = -second / g_FxCommonFrustumLength1;
        limit = (short)threshold;
        if (distance < limit)
            secondVisible = 1;
    }
    if (!thirdVisible) {
        if (third >= 0)
            distance = third / g_FxCommonFrustumLength2;
        else
            distance = -third / g_FxCommonFrustumLength2;
        limit = (short)threshold;
        if (distance < limit)
            thirdVisible = 1;
    }
    if (!fourthVisible) {
        if (fourth >= 0)
            distance = fourth / g_FxCommonFrustumLength3;
        else
            distance = -fourth / g_FxCommonFrustumLength3;
        limit = (short)threshold;
        if (distance < limit)
            fourthVisible = 1;
    }
    return firstVisible & secondVisible & thirdVisible & fourthVisible;
}

/* Sentinel nodes of the three node lists: each head node links back to
 * its tail node, each tail node on to its head. */

void FxCommon_InitNodePools(void)
{
    int i, offset;
    char *slot;

    i = 1;
    slot = (char *)&g_FxCommonRecords[1];
    for (; i < 200; i++, slot += 108)
        FxCommon_ResetTransformNode(slot);

    FxCommon_ResetTransformNode(&g_FxCommonDefaultNode);
    g_FxCommonDefaultNode.bucket = 0;
    g_FxCommonIndexFreeHead = 0;
    for (i = 0; i < 200; i++)
        g_FxCommonIndexFreeNext[i] = i + 1;

    i = 0;
    {
        FxCommonNode *source = g_FxCommonListHeadNodes;
        FxCommonNode *dest = g_FxCommonListTailNodes;

        for (; i < 3;) {
            g_FxCommonListTailNodes[i].previous = source;
            source++;
            g_FxCommonListHeadNodes[i].next = dest;
            dest++;
            g_FxCommonListHeadNodes[i].previous = 0;
            g_FxCommonListTailNodes[i].next = 0;
            i++;
        }
    }

    {
        short empty = -1;

        offset = 0;
        for (; offset < 800; offset += 4) {
            ((FxCommonIndexLink *)&((char *)g_FxCommonIndexLinks)[offset])->previous = empty;
            ((FxCommonIndexLink *)&((char *)g_FxCommonIndexLinks)[offset])->next = empty;
        }
    }

    g_FxCommonIndexLast = 200;
    g_FxCommonIndexLinks[200].previous = -1;
    g_FxCommonIndexLinks[200].next = 201;
    g_FxCommonIndexLinks[201].previous = 200;
    g_FxCommonIndexLinks[201].next = -1;
}

int FxCommon_AllocIndexLink(void);
void FxCommon_LinkNodeToParent(FxCommonNode *node, FxCommonNode *parent);

FxCommonNode *FxCommon_CreateBareNode(FxCommonNode *parent, void *payload)
{
    int index = FxCommon_AllocIndexLink();
    FxCommonNode *node = (FxCommonNode *)&g_FxCommonRecords[(s16)index];

    node->type = 0;
    node->args.payload = payload;
    node->id = index;
    FxCommon_LinkNodeToParent(node, parent);
    node->state[0] = 0;
    node->state[1] = 0;
    node->state[2] = 0;
    return node;
}

int FxCommon_GetOffsetTableValue(void *payload, int type);

FxCommonNode *FxCommon_CreateTypedNode(FxCommonNode *parent, s16 type, void *payload)
{
    int index = FxCommon_AllocIndexLink();
    FxCommonNode *node = (FxCommonNode *)&g_FxCommonRecords[(s16)index];

    node->type = type;
    node->args.payload = payload;
    node->id = index;
    node->value32 = FxCommon_GetOffsetTableValue(payload, type);
    FxCommon_LinkNodeToParent(node, parent);
    node->state[0] = 0;
    node->state[1] = 0;
    node->state[2] = 0;
    return node;
}

int FxCommon_GetOffsetTableValue(void *payload, int subtype);

FxCommonNode *FxCommon_CreateConfiguredNode(FxCommonNode *parent, s16 subtype, s16 variant,
                            s16 type, void *data38, void *data3C,
                            void *payload)
{
    int index = FxCommon_AllocIndexLink();
    FxCommonNode *node = (FxCommonNode *)&g_FxCommonRecords[(s16)index];

    node->args.configured.subtype = subtype;
    node->args.configured.variant = variant;
    node->type = type;
    node->id = index;
    node->args.configured.payload = payload;
    node->value36 = FxCommon_GetOffsetTableValue(payload, subtype);
    node->data38 = data38;
    node->data3C = data3C;
    FxCommon_LinkNodeToParent(node, parent);
    node->state[1] = 0;
    node->state[2] = 0;
    node->state[3] = 0;
    return node;
}

void FxCommon_ReleaseNode(FxCommonNode *node)
{
    FxCommon_UnlinkNode(node);
    FxCommon_FreeIndexLink(node->id);
}

void FxCommon_DrawTransformNode(FxCommonTransformNode *node, void *context)
{
    RotMatrix(&node->seed.room, &node->matrix);
    *g_FxCommonWorkMatrix = node->matrix;
    CompMatrix(&g_FxCommonViewMatrix, g_FxCommonWorkMatrix, g_FxCommonWorkMatrix);
    SetTransMatrix(g_FxCommonWorkMatrix);
    SetRotMatrix(g_FxCommonWorkMatrix);
    FxCommon_DrawPolyResource(context, node->resource);
}

/* Rebuild a transform node's matrix when it is on screen (or forced), apply
 * the mirror matrix for mirrored nodes, then draw it with the renderer that
 * matches its pass and mirror state.  `state` holds the visibility test and
 * then the pass. */
void FxCommon_DrawTransformNodeCulled(FxCommonTransformNode *node, FxCommonBuffer *context, u8 pass, u8 force,
                   u8 mirrored)
{
    GteVector position;
    GteMatrix mirror;
    int state;

    mirror = g_FxCommonMirrorYMatrix;
    position.x = node->matrix.t[0];
    position.y = node->matrix.t[1];
    position.z = node->matrix.t[2];
    state = FxCommon_CheckFourBoundsWithMargin(&position.x, node->margin) | force;
    if (state) {
        RotMatrix(&node->seed.room, &node->matrix);
        if (mirrored == 1) {
            gte_CompMatrix(&mirror, &node->matrix, &node->matrix);
        }
        *g_FxCommonWorkMatrix = node->matrix;
        CompMatrix(&g_FxCommonViewMatrix, g_FxCommonWorkMatrix, g_FxCommonWorkMatrix);
        SetTransMatrix(g_FxCommonWorkMatrix);
        SetRotMatrix(g_FxCommonWorkMatrix);
        state = pass;
        if (state == 0) {
            if (mirrored == 0)
                FxCommon_DrawModel(context, node->resource, node->kind);
            if (mirrored == 1)
                FxCommon_DrawModelTinted(context, node->resource, node->kind);
        }
        if (pass == 1)
            FxCommon_DrawModelFlat(context, node->resource, node->kind);
    }
}

/* Rebuild a level-of-detail node's matrix (mode 2 composes it with the mirror
 * matrix and keeps the original translation), then draw the model kind that
 * matches its view depth when it is on screen or forced. */
void FxCommon_DrawLodNode(FxCommonLodNode *node, FxCommonBuffer *context, u8 force, u8 mode)
{
    GteVector position;
    GteMatrix mirror;
    RoomSpriteMatrix copy;
    RoomFxSeed8 seed;
    s16 margin;
    int depth;

    mirror = g_FxCommonMirrorYMatrix;
    position.x = node->matrix.t[0];
    position.y = node->matrix.t[1];
    position.z = node->matrix.t[2];
    margin = node->margin;
    if (mode == 2) {
        seed = node->seed;
        RotMatrix(&seed, &node->matrix);
        copy = node->matrix;
        gte_CompMatrix(&copy, &mirror, &node->matrix);
        node->matrix.t[0] = copy.t[0];
        node->matrix.t[1] = copy.t[1];
        node->matrix.t[2] = copy.t[2];
    } else {
        RotMatrix(&node->seed, &node->matrix);
    }
    *g_FxCommonWorkMatrix = node->matrix;
    CompMatrix(&g_FxCommonViewMatrix, g_FxCommonWorkMatrix, g_FxCommonWorkMatrix);
    SetTransMatrix(g_FxCommonWorkMatrix);
    SetRotMatrix(g_FxCommonWorkMatrix);
    if (FxCommon_CheckBoundsWithMargin(&position.x, margin) | force) {
        depth = g_FxCommonWorkMatrix->t[2];
        if (depth < node->nearDepth) {
            if (depth > 0x180) {
                if (mode == 0)
                    FxCommon_DrawModel(context, node->resource, node->nearKind);
                if (mode == 1)
                    FxCommon_DrawModelTinted(context, node->resource, node->nearKind);
                if (mode == 2)
                    FxCommon_DrawModelTinted(context, node->resource, node->nearKind);
            }
        } else if (depth < node->farDepth) {
            if (mode == 0)
                FxCommon_DrawModel(context, node->resource, node->middleKind);
            if (mode == 1)
                FxCommon_DrawModelTinted(context, node->resource, node->middleKind);
        } else {
            if (mode == 0)
                FxCommon_DrawModel(context, node->resource, node->farKind);
            if (mode == 1)
                FxCommon_DrawModelTinted(context, node->resource, node->farKind);
            if (mode == 2)
                FxCommon_DrawModelTinted(context, node->resource, node->farKind);
        }
    }
}

void FxCommon_ResetTransformNode(FxCommonTransformNode *node)
{
    node->matrix.t[2] = 1000;
    node->matrix.m[0][0] = 0x1000;
    node->matrix.m[1][1] = 0x1000;
    node->matrix.m[2][2] = 0x1000;
    node->matrix.t[0] = 0;
    node->matrix.t[1] = 0;
    node->matrix.m[0][1] = 0;
    node->matrix.m[0][2] = 0;
    node->matrix.m[1][0] = 0;
    node->matrix.m[1][2] = 0;
    node->matrix.m[2][0] = 0;
    node->matrix.m[2][1] = 0;
    node->seed.component[0] = 0;
    node->seed.component[1] = 0;
    node->seed.component[2] = 0;
    node->record = &g_FxCommonDefaultNode;
    node->previous = 0;
    node->next = 0;
}

int FxCommon_AllocIndexLink(void)
{
    s16 index = g_FxCommonIndexFreeHead;
    s16 current = g_FxCommonIndexLast;
    u16 freeNext;
    u16 next;
    int nextIndex;

    freeNext = g_FxCommonIndexFreeNext[index];
    g_FxCommonIndexLinks[index].previous = current;
    next = g_FxCommonIndexLinks[current].next;
    g_FxCommonIndexLinks[index].next = next;
    g_FxCommonIndexLinks[current].next = index;
    nextIndex = g_FxCommonIndexLinks[index].next;
    g_FxCommonIndexLast = index;
    g_FxCommonIndexFreeHead = freeNext;
    g_FxCommonIndexLinks[nextIndex].previous = index;
    return index;
}

void FxCommon_FreeIndexLink(s16 index)
{
    int freeHead;

    g_FxCommonIndexLinks[g_FxCommonIndexLinks[index].previous].next =
        g_FxCommonIndexLinks[index].next;
    g_FxCommonIndexLinks[g_FxCommonIndexLinks[index].next].previous =
        g_FxCommonIndexLinks[index].previous;
    if (index == g_FxCommonIndexLast) {
        g_FxCommonIndexLast = g_FxCommonIndexLinks[index].previous;
    }
    freeHead = g_FxCommonIndexFreeHead;
    g_FxCommonIndexLinks[index].previous = -1;
    g_FxCommonIndexLinks[index].next = -1;
    g_FxCommonIndexFreeHead = index;
    g_FxCommonIndexFreeNext[index] = freeHead;
}

void FxCommon_BeginRecordIteration(void)
{
    g_FxCommonRecordCursor = 200;
}

FxCommonRecord *FxCommon_NextRecord(void)
{
    int record = g_FxCommonIndexLinks[g_FxCommonRecordCursor].next;

    g_FxCommonRecordCursor = record;
    if (record == 201) {
        record = -1;
    }
    if (record == -1) {
        return 0;
    }
    return &g_FxCommonRecords[record];
}

void FxCommon_LinkNodeToParent(FxCommonNode *node, FxCommonNode *parent)
{
    int bucket;

    node->parent = parent;
    node->bucket = parent->bucket + 1;
    bucket = node->bucket;
    node->previous = g_FxCommonNodeBuckets[bucket].head;
    node->next = node->previous->next;
    g_FxCommonNodeBuckets[bucket].head->next = node;
    g_FxCommonNodeBuckets[bucket].head = node;
}

void FxCommon_UnlinkNode(FxCommonNode *node)
{
    node->previous->next = node->next;
    node->next->previous = node->previous;
}

/* Pick the effect variant, mirror pairs and texture flag for the current
 * room and area before the shared effect assets are set up. */
void FxCommon_SelectRoomVariant(void)
{

    FxCommon_InitDisplay();
    if (Save_GetMetadataWindowIndex() == 0)
        g_FxCommonAltFogLayer = 0;
    else
        g_FxCommonAltFogLayer = 0xFF;
    if (g_GameStateFlagsWord & 0x40000000) {
        g_FxCommonAltFogLayer = 0xFF;
        Menu_SetEquipSlotIndex(1);
    } else {
        g_FxCommonAltFogLayer = 0;
        Menu_SetEquipSlotIndex(0);
    }

    g_FxCommonRoomVariant = -1;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x179)
        g_FxCommonRoomVariant = 6;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x18)
        g_FxCommonRoomVariant = 6;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x2E)
        g_FxCommonRoomVariant = 7;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x75)
        g_FxCommonRoomVariant = 7;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x67)
        g_FxCommonRoomVariant = 7;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0xBF)
        g_FxCommonRoomVariant = 9;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0xC0)
        g_FxCommonRoomVariant = 9;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x5D)
        g_FxCommonRoomVariant = 2;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x60)
        g_FxCommonRoomVariant = 2;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x3A)
        g_FxCommonRoomVariant = 8;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x176)
        g_FxCommonRoomVariant = 8;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x7D)
        g_FxCommonRoomVariant = 4;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x104)
        g_FxCommonRoomVariant = 3;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0xA1)
        g_FxCommonRoomVariant = 1;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0xB7)
        g_FxCommonRoomVariant = 0;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x122)
        g_FxCommonRoomVariant = 5;
    if (g_FxCommonRoomVariant == -1)
        g_FxCommonRoomVariant = 7;

    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x80)
        g_FxCommonStartPreset = 0;
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x208)
        g_FxCommonStartPreset = 7;
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xB8) {
        g_FxCommonConfirmExit = 1;
        g_FxCommonConfirmExitVariant = 9;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xC0) {
        g_FxCommonAutoPlay = 1;
        g_FxCommonAutoVariant = 9;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xD0) {
        g_FxCommonConfirmExit = 1;
        g_FxCommonConfirmExitVariant = 7;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xD8) {
        g_FxCommonAutoPlay = 1;
        g_FxCommonAutoVariant = 7;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xE0) {
        g_FxCommonConfirmExit = 1;
        g_FxCommonConfirmExitVariant = 8;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xE4) {
        g_FxCommonAutoPlay = 1;
        g_FxCommonAutoVariant = 8;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x148) {
        g_FxCommonConfirmExit = 1;
        g_FxCommonConfirmExitVariant = 9;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x160) {
        g_FxCommonAutoPlay = 1;
        g_FxCommonAutoVariant = 9;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x178) {
        g_FxCommonConfirmExit = 1;
        g_FxCommonConfirmExitVariant = 7;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x180) {
        g_FxCommonAutoPlay = 1;
        g_FxCommonAutoVariant = 7;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x1C0) {
        g_FxCommonConfirmExit = 1;
        g_FxCommonConfirmExitVariant = 4;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x1C8) {
        g_FxCommonAutoPlay = 1;
        g_FxCommonAutoVariant = 4;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] & 0x2000)
        g_FxCommonNight = 1;
    else
        g_FxCommonNight = 0;
    FxCommon_LoadEffectSectors();
}

void FxCommon_LoadEffectSectors(void) {
    FxAssetSource *resource = (FxAssetSource *)&g_GameStateFlagsWord;
    u16 *range;
    int result;

first:
    range = &g_PeImageSectorTable[76];
    do {
        result = CdRom_ReadSectorsFromLba(resource->data + range[0],
                               &g_FxCommonTimTable, range[1] - range[0]);
    } while (result == -1);
    for (;;) {
        result = CdRom_PollReady();
        if (result == 0) break;
        if (result == -1) goto first;
    }

    if (!(g_FxCommonNight)) {

second:
    range = &g_PeImageSectorTable[78];
    do {
        result = CdRom_ReadSectorsFromLba(resource->data + range[0],
                               &g_FxCommonModelTable, range[1] - range[0]);
    } while (result == -1);
    for (;;) {
        result = CdRom_PollReady();
        if (result == 0) return;
        if (result == -1) goto second;
    }

    }
third:
    range = &g_PeImageSectorTable[77];
    do {
        result = CdRom_ReadSectorsFromLba(resource->data + range[0],
                               &g_FxCommonModelTable, range[1] - range[0]);
    } while (result == -1);
    for (;;) {
        result = CdRom_PollReady();
        if (result == 0) break;
        if (result == -1) goto third;
    }
}

void FxCommon_SetAkao3dVolumeRange(int extended)
{
    if (extended == 0) {
        g_Akao3dVolumeMin = 0x10;
        g_Akao3dVolumeMax = 0x10;
    } else {
        g_Akao3dVolumeMin = 0x10;
        g_Akao3dVolumeMax = 0x40;
    }
    g_GameState.bank_value_f8 = 0;
    g_GameState.bank_value_fa = 0xFFF;
}

int FxCommon_SendTableCommand3d(int id, s32 *state)
{
    int size = 0x300;
    void *resource;
    GteMatrix **matrixSlot;
    GteMatrix *savedMatrix;
    s32 *savedOutput;
    int result;

    resource = Draw_GetTimTableEntry(&g_FxCommonTimTable, 3);
    PushMatrix();
    SetTransMatrix(&g_FxCommonViewMatrix);
    SetRotMatrix(&g_FxCommonViewMatrix);
    matrixSlot = &g_ViewMatrixSlot.value;
    savedMatrix = *matrixSlot;
    *matrixSlot = &g_FxCommonViewMatrix;
    savedOutput = g_ProjectionSlot.distance;
    g_ProjectionSlot.distance = &size;
    result = Akao_SendTableCommand(resource, id, 0, 0x80, 1);
    *matrixSlot = savedMatrix;
    g_ProjectionSlot.distance = savedOutput;
    PopMatrix();
    return result;
}

void FxCommon_Apply3dPan(int handle, volatile s32 *state)
{
    struct FxCommonApplyScratch {
        int size;
        int pad14;
        AkaoPackedRect3 vector;
        s16 pad1E;
        int valueA;
        int valueB;
    } scratch;
    GteMatrix **matrixSlot;
    GteMatrix *savedMatrix;
    s32 *savedOutput;

    scratch.size = 0x300;
    PushMatrix();
    SetTransMatrix(&g_FxCommonViewMatrix);
    SetRotMatrix(&g_FxCommonViewMatrix);
    matrixSlot = &g_ViewMatrixSlot.value;
    savedMatrix = *matrixSlot;
    *matrixSlot = &g_FxCommonViewMatrix;
    savedOutput = g_ProjectionSlot.distance;
    g_ProjectionSlot.distance = &scratch.size;
    scratch.vector.x = state[0];
    scratch.vector.y = state[1];
    scratch.vector.z = state[2];
    Akao_Calc3DPan(&scratch.vector, &scratch.valueA, &scratch.valueB);
    Akao_Cmd_A0(handle, 0, scratch.valueB);
    Akao_Cmd_A2(handle, 0, scratch.valueA);
    *matrixSlot = savedMatrix;
    g_ProjectionSlot.distance = savedOutput;
    PopMatrix();
}
