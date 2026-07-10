/* CC1_FLAGS: -G8 */
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

typedef struct Vec3i {
    s32 x;
    s32 y;
    s32 z;
} Vec3i;

typedef signed short s16_3;
typedef unsigned short u16;
typedef int s32_3;

void Entity_SlideOnWall(void *entity, s32_3 points_addr, u16 count, s16_3 edge_index, s16_3 saved_x, s16_3 saved_y);
s16_3 Geo_FindNearestEdge(s16_3, s16_3, s32_3, u16);

extern u16 g_EntityCollisionMoveDelta;
extern u16 g_EntityCollisionWallParam;
extern s32_3 g_EntityCollisionWallSlot;
extern u16 D_8009CE2C;
extern struct { char _[16]; } g_PlayerEntity_0_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_0 (*(void **)&g_PlayerEntity_0_o)
extern struct { char _[16]; } g_PlayerEntity_1_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_1 (*(void **)&g_PlayerEntity_1_o)
extern struct { char _[16]; } g_PlayerEntity_2_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_2 (*(void **)&g_PlayerEntity_2_o)
extern struct { char _[16]; } g_PlayerEntity_3_o __asm__("g_PlayerEntity");
#define g_PlayerEntity_3 (*(void **)&g_PlayerEntity_3_o)

int Gte_ISqrt(int arg0);
void Gte_NormalizeVec(Vec3i *src, Vec3i *dst);
int Math_FixedMul(int arg0, int arg1);

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

void Entity_SlideOnWall(void *entity, s32_3 points_addr, u16 count, s16_3 edge_index, s16_3 saved_x, s16_3 saved_y) {
    PolyPoint *points;
    Vec3i edge;
    Vec3i normal;
    int cur_x;
    int cur_y;
    int prev_x;
    int prev_y;
    int dx;
    int dy;
    int projected_x;
    int projected_y;
    int distance;
    int radius;
    int step;
    int dir;
    int candidate_x;
    int candidate_y;
    int edge_len;
    int projection;

    (void)saved_x;
    (void)saved_y;

    points = (PolyPoint *)points_addr;
    cur_x = points[edge_index].x;
    cur_y = points[edge_index].y;
    if (edge_index > 0) {
        prev_x = points[edge_index - 1].x;
        prev_y = points[edge_index - 1].y;
    } else {
        prev_x = points[count - 1].x;
        prev_y = points[count - 1].y;
    }

    edge.x = cur_x - prev_x;
    edge.y = 0;
    edge.z = cur_y - prev_y;
    Gte_NormalizeVec(&edge, &edge);
    normal = edge;
    normal.x <<= 4;
    normal.z <<= 4;

    projection = Math_FixedMul(normal.x, *(s32 *)((char *)entity + 0x28) - *(s32 *)((char *)entity + 0x40));
    projection += Math_FixedMul(normal.z, *(s32 *)((char *)entity + 0x30) - *(s32 *)((char *)entity + 0x48));
    projected_x = Math_FixedMul(normal.x, projection) + *(s32 *)((char *)entity + 0x40);
    projected_y = Math_FixedMul(normal.z, projection) + *(s32 *)((char *)entity + 0x48);

    dx = prev_x - cur_x;
    dy = prev_y - cur_y;
    edge.x = dx;
    edge.y = 0;
    edge.z = dy;
    edge_len = Gte_ISqrt(dx * dx + dy * dy);

    distance = ((((projected_y >> 16) - cur_y) * dx) - (((projected_x >> 16) - cur_x) * dy)) / edge_len;
    if (distance < 0) {
        distance = -distance;
    }

    radius = D_8009CE2C;
    if (radius < distance) {
        *(s32 *)((char *)entity + 0x28) = projected_x;
        *(s32 *)((char *)entity + 0x30) = projected_y;
        return;
    }

    step = 0;
    while (1) {
        int step_fixed = step << 16;
        for (dir = 0; dir < 4; dir++) {
            candidate_x = projected_x;
            candidate_y = projected_y;
            if (dir == 0) {
                candidate_x += step_fixed;
            } else if (dir == 1) {
                candidate_x -= step_fixed;
            } else if (dir == 2) {
                candidate_y += step_fixed;
            } else {
                candidate_y -= step_fixed;
            }

            distance = ((((candidate_y >> 16) - cur_y) * dx) - (((candidate_x >> 16) - cur_x) * dy)) / edge_len;
            if (distance < 0) {
                distance = -distance;
            }

            if (radius < distance) {
                *(s32 *)((char *)entity + 0x28) = candidate_x;
                *(s32 *)((char *)entity + 0x30) = candidate_y;
                return;
            }
        }
        step++;
    }
}

s16_3 Geo_FindNearestEdge(s16_3 x, s16_3 y, s32_3 points_addr, u16 count) {
    PolyPoint *points;
    Vec3i edge;
    Vec3i normal;
    int prev_x;
    int prev_y;
    int cur_x;
    int cur_y;
    int dx;
    int dy;
    int point_dx;
    int point_dy;
    int edge_len;
    int distance;
    int projection;
    int radius;
    u16 i;

    points = (PolyPoint *)points_addr;
    radius = D_8009CE2C;
    prev_x = points[count - 1].x;
    prev_y = points[count - 1].y;

    for (i = 0; i < count; i++) {
        cur_x = points[i].x;
        cur_y = points[i].y;

        if (cur_x < x - radius && prev_x < x - radius) {
            prev_x = cur_x;
            prev_y = cur_y;
            continue;
        }
        if (x + radius < cur_x && x + radius < prev_x) {
            prev_x = cur_x;
            prev_y = cur_y;
            continue;
        }
        if (cur_y < y - radius && prev_y < y - radius) {
            prev_x = cur_x;
            prev_y = cur_y;
            continue;
        }
        if (y + radius < cur_y && y + radius < prev_y) {
            prev_x = cur_x;
            prev_y = cur_y;
            continue;
        }

        dx = prev_x - cur_x;
        dy = prev_y - cur_y;
        edge.x = dx;
        edge.y = 0;
        edge.z = dy;
        edge_len = Gte_ISqrt(dx * dx + dy * dy);

        point_dx = x - cur_x;
        point_dy = y - cur_y;
        distance = ((point_dy * dx) - (point_dx * dy)) / edge_len;
        if (distance < 0) {
            distance = -distance;
        }

        if (distance <= radius) {
            Gte_NormalizeVec(&edge, &edge);
            normal = edge;
            projection = Math_FixedMul(normal.x << 4, point_dx << 16);
            projection += Math_FixedMul(normal.z << 4, point_dy << 16);

            if (projection < 0) {
                if ((point_dx * point_dx) + (point_dy * point_dy) <= radius * radius) {
                    return i;
                }
            } else if (edge_len < (projection >> 16)) {
                point_dx = x - prev_x;
                point_dy = y - prev_y;
                if ((point_dx * point_dx) + (point_dy * point_dy) > radius * radius) {
                    prev_x = cur_x;
                    prev_y = cur_y;
                    continue;
                }
            } else {
                return i;
            }
        }

        prev_x = cur_x;
        prev_y = cur_y;
    }

    return -1;
}

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
