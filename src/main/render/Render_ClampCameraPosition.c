#include "common.h"
#include "pe1/geom_state.h"

extern s32 g_RenderStateFlags;
extern s16 g_CameraClampMinX __asm__("D_800BCF8C");
extern s16 g_CameraClampedY;

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
    CameraViewport *entry;
    s32 sy;
    GeomState *base;
    s32 clamped;
    CameraViewport *bounds;

    sx = x;
    if ((g_RenderStateFlags & 0x40) != 0) {
        original_x = x;
        base = g_GeomState;
        entry = (CameraViewport *)g_GeomState;
        entry = (CameraViewport *)((char *)entry + base->entry_offset_1C);
        entry = &entry[g_GeomGroupSel];
        bounds = entry;

        if ((s16)sx < bounds->minX) {
            sx = entry->minX;
            g_CameraClampMinX = sx;
        } else if (bounds->maxX < (s16)sx) {
            sx = bounds->maxX;
            g_CameraClampMinX = sx;
        } else {
            g_CameraClampMinX = original_x;
        }

        sy = (s16)y;
        if (sy < bounds->minY) {
            clamped = (g_CameraClampedY = bounds->minY);
            return 0;
        }
        if (bounds->maxY < sy) {
            g_CameraClampedY = bounds->maxY;
            return 0;
        }
        g_CameraClampedY = y;
    }
    return 0;
}
