/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed short s16;
typedef unsigned short u16;
typedef int s32;

typedef struct GeoState {
    char pad0[0x18];
    void *points;
    void *polys;
} GeoState;

extern s32 D_8009D1D8;
extern GeoState *D_8009D1FC;

int Geo_PointInTri(void *poly, int x, int y);

#define U16_AT(ptr, offset) (*(u16 *)((char *)(ptr) + (offset)))
#define VALUE_IN_DENOM_RANGE(value, denom) \
    (((denom) > 0) ? ((value) >= 0 && (value) <= (denom)) : ((value) <= 0 && (denom) <= (value)))

void *Geo_ClipToFloorBoundarySub(void *poly, void *previous_poly, s16 start_x, s16 start_y, s16 end_x, s16 end_y) {
    int move_dx;
    int move_dy;
    int max_x;
    int min_x;
    int max_y;
    int min_y;
    int prev_x;
    int prev_y;
    int cur_x;
    int cur_y;
    int edge;
    s16 point_x;
    s16 point_y;
    char *points;
    char *polys;
    u16 point_index;
    u16 poly_index;

    move_dx = end_x - start_x;
    move_dy = end_y - start_y;

    if (move_dx < 0) {
        max_x = start_x;
        min_x = end_x;
    } else {
        max_x = end_x;
        min_x = start_x;
    }

    if (move_dy < 0) {
        max_y = start_y;
        min_y = end_y;
    } else {
        max_y = end_y;
        min_y = start_y;
    }

    points = D_8009D1FC->points;
    if (D_8009D1D8 != 0) {
        point_index = U16_AT(poly, 0xC);
        point_x = *(s16 *)(points + point_index * 6);
        point_y = *(s16 *)(points + point_index * 6 + 4);
    } else {
        point_index = U16_AT(poly, 6);
        point_x = *(s16 *)(points + point_index * 4);
        point_y = *(s16 *)(points + point_index * 4 + 2);
    }
    prev_x = point_x;
    prev_y = point_y;

    for (edge = 0; edge < 3; edge++) {
        int edge_dx;
        int edge_dy;
        int rel_x;
        int rel_y;
        int denom;
        int hit_a;
        int hit_b;
        void *neighbor;
        void *result;

        points = D_8009D1FC->points;
        if (D_8009D1D8 != 0) {
            point_index = U16_AT(poly, 8 + edge * 2);
            point_x = *(s16 *)(points + point_index * 6);
            point_y = *(s16 *)(points + point_index * 6 + 4);
        } else {
            point_index = U16_AT(poly, 2 + edge * 2);
            point_x = *(s16 *)(points + point_index * 4);
            point_y = *(s16 *)(points + point_index * 4 + 2);
        }
        cur_x = point_x;
        cur_y = point_y;

        edge_dx = cur_x - prev_x;
        if (edge_dx > 0) {
            if (max_x < prev_x || min_x >= cur_x) {
                prev_x = cur_x;
                prev_y = cur_y;
                continue;
            }
        } else {
            if (max_x < cur_x || min_x >= prev_x) {
                prev_x = cur_x;
                prev_y = cur_y;
                continue;
            }
        }

        edge_dy = cur_y - prev_y;
        if (edge_dy > 0) {
            if (max_y < prev_y || min_y >= cur_y) {
                prev_x = cur_x;
                prev_y = cur_y;
                continue;
            }
        } else {
            if (max_y < cur_y || min_y >= prev_y) {
                prev_x = cur_x;
                prev_y = cur_y;
                continue;
            }
        }

        rel_x = start_x - cur_x;
        rel_y = start_y - cur_y;
        denom = move_dy * edge_dx - move_dx * edge_dy;
        hit_a = edge_dy * rel_x - edge_dx * rel_y;
        if (!VALUE_IN_DENOM_RANGE(hit_a, denom)) {
            prev_x = cur_x;
            prev_y = cur_y;
            continue;
        }

        hit_b = move_dx * rel_y - move_dy * rel_x;
        if (!VALUE_IN_DENOM_RANGE(hit_b, denom) || denom == 0) {
            prev_x = cur_x;
            prev_y = cur_y;
            continue;
        }

        polys = D_8009D1FC->polys;
        if (D_8009D1D8 != 0) {
            poly_index = U16_AT(poly, 0x14 + edge * 2);
            neighbor = polys + poly_index * 28;
        } else {
            poly_index = U16_AT(poly, 0xE + edge * 2);
            neighbor = polys + poly_index * 22;
        }
        if (neighbor == previous_poly) {
            prev_x = cur_x;
            prev_y = cur_y;
            continue;
        }
        if (Geo_PointInTri(neighbor, start_x, start_y) != 0) {
            return neighbor;
        }

        result = Geo_ClipToFloorBoundarySub(neighbor, poly, start_x, start_y, end_x, end_y);
        if (result != 0) {
            return result;
        }

        prev_x = cur_x;
        prev_y = cur_y;
    }

    return 0;
}
