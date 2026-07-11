/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

typedef struct GeoState {
    char pad0[0x18];
    void *points;
    void *polys;
} GeoState;

extern s32 D_8009D1D8;
extern GeoState *D_8009D1FC;
extern u16 D_8009CE2C;
extern s16 D_8009CE0C;
extern s16 D_8009CE0E;
extern s16 D_8009CE10;
extern s16 D_8009CE12;
extern s16 D_8009CE18;
extern s16 D_8009CE1C;
extern s16 D_8009CE20;
extern s16 D_8009CE24;
extern s16 D_8009CE28;
extern u32 D_8009DFB0[];

int Geo_PointInTri(void *poly, int x, int y);
void *Geo_ClipToFloorBoundarySub(void *poly, void *previous_poly, s16 start_x, s16 start_y, s16 end_x, s16 end_y);

#define U16_AT(ptr, offset) (*(u16 *)((char *)(ptr) + (offset)))

static int Geo_PolyStride(void) {
    return D_8009D1D8 != 0 ? 28 : 22;
}

static int Geo_PointStride(void) {
    return D_8009D1D8 != 0 ? 6 : 4;
}

static u16 Geo_StartPointIndex(void *poly) {
    if (D_8009D1D8 != 0) {
        return U16_AT(poly, 0xC);
    }
    return U16_AT(poly, 6);
}

static u16 Geo_PointIndex(void *poly, int edge) {
    if (D_8009D1D8 != 0) {
        return U16_AT(poly, 8 + edge * 2);
    }
    return U16_AT(poly, 2 + edge * 2);
}

static u16 Geo_NeighborIndex(void *poly, int edge) {
    if (D_8009D1D8 != 0) {
        return U16_AT(poly, 0x14 + edge * 2);
    }
    return U16_AT(poly, 0xE + edge * 2);
}

static char *Geo_PointPtr(u16 point_index) {
    return (char *)D_8009D1FC->points + point_index * Geo_PointStride();
}

static s16 Geo_PointX(u16 point_index) {
    return *(s16 *)Geo_PointPtr(point_index);
}

static s16 Geo_PointY(u16 point_index) {
    char *point;

    point = Geo_PointPtr(point_index);
    if (D_8009D1D8 != 0) {
        return *(s16 *)(point + 4);
    }
    return *(s16 *)(point + 2);
}

static void *Geo_PolyPtr(u16 poly_index) {
    return (char *)D_8009D1FC->polys + poly_index * Geo_PolyStride();
}

static int Geo_PolyIndex(void *poly) {
    return ((char *)poly - (char *)D_8009D1FC->polys) / Geo_PolyStride();
}

static int Geo_Abs(int value) {
    if (value < 0) {
        return -value;
    }
    return value;
}

static int Geo_MarkVisited(void *poly) {
    u32 index;
    u32 mask;

    index = Geo_PolyIndex(poly);
    mask = 1 << (index & 0x1F);
    if ((D_8009DFB0[index >> 5] & mask) != 0) {
        return 1;
    }
    D_8009DFB0[index >> 5] |= mask;
    return 0;
}

static int Geo_EdgeNearPoint(int x, int y, int x0, int y0, int x1, int y1, int radius) {
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int dx;
    int dy;
    int cross;

    if (x0 < x1) {
        min_x = x0;
        max_x = x1;
    } else {
        min_x = x1;
        max_x = x0;
    }

    if (y0 < y1) {
        min_y = y0;
        max_y = y1;
    } else {
        min_y = y1;
        max_y = y0;
    }

    if (x + radius < min_x || x - radius > max_x || y + radius < min_y || y - radius > max_y) {
        return 0;
    }

    dx = x1 - x0;
    dy = y1 - y0;
    cross = (x - x0) * dy - (y - y0) * dx;
    cross = Geo_Abs(cross);

    return cross <= radius * (Geo_Abs(dx) + Geo_Abs(dy) + 1);
}

static void Geo_RecordBoundary(int poly_index, int x0, int y0, int x1, int y1) {
    D_8009CE0C = x0;
    D_8009CE0E = y0;
    D_8009CE10 = x1;
    D_8009CE12 = y1;

    if (x1 < x0) {
        D_8009CE1C = x1;
        D_8009CE20 = x0;
    } else {
        D_8009CE1C = x0;
        D_8009CE20 = x1;
    }

    if (y1 < y0) {
        D_8009CE24 = y1;
        D_8009CE28 = y0;
    } else {
        D_8009CE24 = y0;
        D_8009CE28 = y1;
    }

    D_8009CE18 = poly_index;
}

int Geo_ClipToFloorBoundary(s16 x, s16 y, void *poly) {
    int edge;
    int radius;
    int prev_x;
    int prev_y;
    int cur_x;
    int cur_y;
    int poly_index;
    u16 point_index;

    if (poly == 0) {
        return 1;
    }

    if (Geo_MarkVisited(poly) != 0) {
        return 1;
    }

    if (Geo_PointInTri(poly, x, y) != 0) {
        return 0;
    }

    radius = D_8009CE2C;
    poly_index = Geo_PolyIndex(poly);
    point_index = Geo_StartPointIndex(poly);
    prev_x = Geo_PointX(point_index);
    prev_y = Geo_PointY(point_index);

    for (edge = 0; edge < 3; edge++) {
        u16 neighbor_index;
        void *neighbor;

        point_index = Geo_PointIndex(poly, edge);
        cur_x = Geo_PointX(point_index);
        cur_y = Geo_PointY(point_index);

        if (Geo_EdgeNearPoint(x, y, prev_x, prev_y, cur_x, cur_y, radius) != 0) {
            neighbor_index = Geo_NeighborIndex(poly, edge);
            if (neighbor_index == 0xFFFF) {
                Geo_RecordBoundary(poly_index, prev_x, prev_y, cur_x, cur_y);
                return 0;
            }

            neighbor = Geo_PolyPtr(neighbor_index);
            if ((*(s8 *)neighbor & 0x80) != 0) {
                Geo_RecordBoundary(poly_index, prev_x, prev_y, cur_x, cur_y);
                return 0;
            }

            if (Geo_PointInTri(neighbor, x, y) != 0) {
                return 0;
            }

            if (Geo_ClipToFloorBoundarySub(neighbor, poly, x, y, prev_x, prev_y) != 0) {
                return 0;
            }
        }

        prev_x = cur_x;
        prev_y = cur_y;
    }

    return 1;
}
