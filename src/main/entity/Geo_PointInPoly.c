/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct PolyPoint {
    s16 pad0;
    s16 x;
    s16 pad4;
    s16 y;
} PolyPoint;

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
