typedef unsigned char u8;
typedef unsigned short u16;

extern int D_8009D1D8;
extern void *D_8009D1FC;

#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(short *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(void **)((u8 *)(ptr) + (off)))

int Geo_PointInTri(void *poly, int x, int y) {
    u8 *cursor;
    u8 *verts;
    u16 curr_x;
    u16 curr_y;
    u16 next_x;
    u16 next_y;
    int inside;
    int i;
    int sx;
    int sy;

    cursor = poly;
    if (D_8009D1D8 != 0) {
        verts = PTR_AT(D_8009D1FC, 0x18);
        curr_x = U16_AT(verts + (U16_AT(cursor, 0xC) * 6), 0);
        curr_y = U16_AT(verts + (U16_AT(cursor, 0xC) * 6), 4);
    } else {
        verts = PTR_AT(D_8009D1FC, 0x18);
        curr_x = U16_AT(verts + (U16_AT(cursor, 6) * 4), 0);
        curr_y = U16_AT(verts + (U16_AT(cursor, 6) * 4), 2);
    }

    inside = 0;
    sx = (short)x;
    sy = (short)y;
    for (i = 0; i < 3; i++, cursor += 2) {
        int old_x = curr_x;
        int old_y = curr_y;

        if (D_8009D1D8 != 0) {
            u8 *v = verts + (U16_AT(cursor, 8) * 6);
            next_x = U16_AT(v, 0);
            next_y = U16_AT(v, 4);
        } else {
            u8 *v = verts + (U16_AT(cursor, 2) * 4);
            next_x = U16_AT(v, 0);
            next_y = U16_AT(v, 2);
        }

        if ((((short)next_y > sy) && !((short)old_y > sy)) ||
            (!((short)next_y > sy) && ((short)old_y > sy))) {
            if ((((short)next_x > sx) && ((short)old_x > sx)) ||
                ((((short)next_x > sx) || ((short)old_x > sx)) &&
                 (((short)old_y - (short)next_y) < 0
                      ? (((short)old_x - (short)next_x) * (sy - (short)next_y) <
                         ((short)old_y - (short)next_y) * (sx - (short)next_x))
                      : (((short)old_y - (short)next_y) * (sx - (short)next_x) <
                         ((short)old_x - (short)next_x) * (sy - (short)next_y))))) {
                inside = inside < 1;
            }
        }

        curr_x = next_x;
        curr_y = next_y;
    }

    return inside;
}
