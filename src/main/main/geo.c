/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "include_asm.h"

typedef signed short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct PolyPoint {
    s16 pad0;
    s16 x;
    s16 pad4;
    s16 y;
} PolyPoint;

typedef signed short s16_3;
typedef unsigned short u16;
typedef int s32_3;

void Entity_SlideOnWall();
s16_3 Geo_FindNearestEdge(s16_3, s16_3, s32_3, u16);

extern u16 g_EntityCollisionMoveDelta;
extern u16 g_EntityCollisionWallParam;
extern s32_3 g_EntityCollisionWallSlot;
extern struct { char _[16]; } g_PlayerEntity_0_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_0 (*(void **)&g_PlayerEntity_0_o)
extern struct { char _[16]; } g_PlayerEntity_1_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_1 (*(void **)&g_PlayerEntity_1_o)
extern struct { char _[16]; } g_PlayerEntity_2_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_2 (*(void **)&g_PlayerEntity_2_o)
extern struct { char _[16]; } g_PlayerEntity_3_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_3 (*(void **)&g_PlayerEntity_3_o)

s32 Geo_PointInPoly(s32 arg0, s32 arg1, PolyPoint *points, s32 count) {
    s32 x;
    s32 y;
    PolyPoint *poly;
    u32 n;
    s32 y_lt_cur;
    s32 prev_y;
    register s32 cur_x asm("$10");
    s32 inside;
    register s32 cur_y asm("$12");
    register s32 i asm("$13");
    s32 old_prev_x;
    s32 lhs;
    register s32 rhs asm("$8");
    register s32 x_lt_cur asm("$8");
    s32 tmp;

    x = arg0 >> 16;
    asm volatile("" : "=r"(x) : "0"(x));
    y = arg1 >> 16;
    asm volatile("" : "=r"(y) : "0"(y));
    poly = points;
    i = 0;
    inside = 0;
    n = count & 0xFFFF;
    cur_x = poly[n - 1].x;
    cur_y = poly[n - 1].y;
    asm volatile("" : "=r"(cur_y) : "0"(cur_y));
    prev_y = cur_y;

    do {
        tmp = (i & 0xFFFF) << 3;
        tmp = tmp + (s32) poly;
        old_prev_x = cur_x;
        cur_y = ((PolyPoint *) tmp)->y;
        cur_x = ((PolyPoint *) tmp)->x;
        y_lt_cur = y < cur_y;
        tmp = y < prev_y;

        if (!y_lt_cur) {
            if (tmp) {
                goto edge;
            }
            if (!y_lt_cur) {
                goto next;
            }
        }
        if (tmp) {
            goto next;
        }
edge:
        x_lt_cur = x < cur_x;
        if (!x_lt_cur) {
            goto check_prev_x;
        }
        tmp = x < old_prev_x;
        if (tmp) {
            goto toggle;
        }
        if (x_lt_cur) {
            goto crossing;
        }
check_prev_x:
        tmp = x < old_prev_x;
        if (tmp) {
            goto crossing;
        } else {
            goto next;
        }
crossing:
        old_prev_x -= cur_x;
        tmp = y - cur_y;
        lhs = old_prev_x * tmp;
        y_lt_cur = prev_y - cur_y;
        tmp = x - cur_x;
        if (y_lt_cur < 0) {
            rhs = y_lt_cur * tmp;
            tmp = lhs < rhs;
        } else {
            rhs = y_lt_cur * tmp;
            tmp = rhs < lhs;
        }
        if (tmp) {
toggle:
            inside = inside == 0;
        }

next:
        i += 1;
        prev_y = cur_y;
    } while ((u32)(i & 0xFFFF) < n);

    return inside;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/main/geo", Entity_SlideOnWall);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/geo", Geo_FindNearestEdge);

void Entity_ApplyCollisionResponse(void) {
    char *obj;
    s32_3 saved_42;
    s32_3 saved_4a;
    s32_3 product;
    u16 value_224;
    register char *obj2 asm("$2");
    register s32_3 arg0 asm("$4");
    register s32_3 arg1 asm("$5");
    register s32_3 arg2 asm("$6");
    register u16 arg3 asm("$7");
    s16_3 hit;
    s32_3 copy_40;
    s32_3 copy_44;
    s32_3 copy_48;

    obj = g_PlayerEntity_0;
    value_224 = *(u16 *)(obj + 0x224);
    product = value_224;
    product *= *(u16 *)(obj + 0x26);
    g_EntityCollisionMoveDelta = value_224;
    if (product < 0) {
        product += 0xFFF;
    }

    arg0 = *(s16_3 *)(obj + 0x2A);
    arg1 = *(s16_3 *)(obj + 0x32);
    arg2 = g_EntityCollisionWallSlot;
    arg3 = g_EntityCollisionWallParam;
    saved_42 = *(s16_3 *)(obj + 0x42);
    saved_4a = *(s16_3 *)(obj + 0x4A);
    g_EntityCollisionMoveDelta = (u16)(product >> 12);
    hit = Geo_FindNearestEdge(arg0, arg1, arg2, arg3);
    if (hit < 0) {
        return;
    }

    Entity_SlideOnWall(g_PlayerEntity_1, g_EntityCollisionWallSlot, g_EntityCollisionWallParam, hit, saved_42, saved_4a);
    arg2 = g_EntityCollisionWallSlot;
    obj2 = g_PlayerEntity_2;
    arg3 = g_EntityCollisionWallParam;
    if (Geo_FindNearestEdge(
            *(s16_3 *)(obj2 + 0x2A),
            *(s16_3 *)(obj2 + 0x32),
            arg2,
            arg3) >= 0) {
        obj2 = g_PlayerEntity_3;
        copy_40 = *(s32_3 *)(obj2 + 0x40);
        copy_44 = *(s32_3 *)(obj2 + 0x44);
        copy_48 = *(s32_3 *)(obj2 + 0x48);
        *(s32_3 *)(obj2 + 0x28) = copy_40;
        *(s32_3 *)(obj2 + 0x2C) = copy_44;
        *(s32_3 *)(obj2 + 0x30) = copy_48;
    }
}
