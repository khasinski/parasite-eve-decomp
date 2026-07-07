/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

typedef struct {
    char pad[0x2C];
    s16 min_x;
    s16 max_x;
    s16 min_y;
    s16 max_y;
} UnkBounds;

typedef struct {
    char pad[0x1C];
    s32 bounds_offset;
} UnkState;

extern s32 g_RenderStateFlags;
extern s16 D_800BCF8C;
extern s16 g_CameraClampedY;
extern u8 g_GeomGroupSel;
extern UnkState * volatile g_GeomState;

/* Several shapes below are load-bearing for the byte-match (permuter zero):
 * the unused[1] pad keeps retail's empty 0x20 stack frame, `unsigned short sx`
 * moves x out of $a0 so `bounds` can take it, the `bounds = entry` copy splits
 * the pointer live range, and the assignment-in-assignment on min_y plus the
 * doubled max_y field read reproduce retail's clamp-value register copies. */
s32 Render_ClampCameraPosition(s32 x, s32 y)
{
    char unused[0x1];
    unsigned short sx;
    s32 original_x;
    UnkBounds *entry;
    s32 sy;
    UnkState *base;
    s32 clamped;
    UnkBounds *bounds;

    sx = x;
    if ((g_RenderStateFlags & 0x40) != 0) {
        original_x = x;
        base = g_GeomState;
        entry = (UnkBounds *)g_GeomState;
        entry = (UnkBounds *)((char *)entry + base->bounds_offset);
        entry = &entry[g_GeomGroupSel];
        bounds = entry;

        if ((s16)sx < bounds->min_x) {
            sx = entry->min_x;
            D_800BCF8C = sx;
        } else if (bounds->max_x < (s16)sx) {
            sx = bounds->max_x;
            D_800BCF8C = sx;
        } else {
            D_800BCF8C = original_x;
        }

        sy = (s16)y;
        if (sy < bounds->min_y) {
            clamped = (g_CameraClampedY = bounds->min_y);
            return 0;
        }
        if (bounds->max_y < sy) {
            g_CameraClampedY = bounds->max_y;
            return 0;
        }
        g_CameraClampedY = y;
    }
    return 0;
}
