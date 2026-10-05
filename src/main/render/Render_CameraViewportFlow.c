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
extern char * volatile g_GeomStateBytes __asm__("g_GeomState");
extern unsigned char g_GeomGroupSel;
extern unsigned short D_800BCFAC;
extern unsigned short g_RenderSavedBoundsMaxX;
extern unsigned short D_800BCFB0;
extern unsigned short g_RenderSavedBoundsMaxY;

int Render_SaveAndOpenBounds(void) {
    char *base = g_GeomStateBytes;
    char *entry = g_GeomStateBytes + *(int *)(base + 0x1C) + (g_GeomGroupSel * 52);

    D_800BCFAC = *(unsigned short *)(entry + 0x2C);
    g_RenderSavedBoundsMaxX = *(unsigned short *)(entry + 0x2E);
    D_800BCFB0 = *(unsigned short *)(entry + 0x30);
    g_RenderSavedBoundsMaxY = *(unsigned short *)(entry + 0x32);

    *(short *)(entry + 0x2C) = -0x8000;
    *(short *)(entry + 0x2E) = 0x7FFF;
    *(short *)(entry + 0x30) = -0x8000;
    *(short *)(entry + 0x32) = 0x7FFF;

    return 0;
}

int Render_RestoreBounds(void) {
    char *base = g_GeomStateBytes;
    char *entry = g_GeomStateBytes + *(int *)(base + 0x1C) + (g_GeomGroupSel * 52);

    *(unsigned short *)(entry + 0x2C) = D_800BCFAC;
    *(unsigned short *)(entry + 0x2E) = g_RenderSavedBoundsMaxX;
    *(unsigned short *)(entry + 0x30) = D_800BCFB0;
    *(unsigned short *)(entry + 0x32) = g_RenderSavedBoundsMaxY;

    return 0;
}
/* Matching debt: register pins, empty scheduling constraints, a $v1 clobber,
 * explicit signed-halving steps, gotos and 40 bytes of frame padding. */
#include "pe1/geom_state.h"
#include "pe1/render_camera.h"
#include "pe1/gte.h"
#include "pe1/psyq_nop.h"

extern int *D_800BCFA8;

int Render_SetViewport(s16 *position) {
    struct {
        s16 point[4];
        s16 projected[2];
        unsigned reserved[10];
    } local;
    GeomScrollState *camera = &D_800BCF88;
    register GeomState *geometry;
    register CameraViewport *view;
    register unsigned int cx asm("$11");
    register int correction asm("$2");
    register unsigned fallback_height asm("$2");
    register int fallback_half asm("$3");
    register int height_center asm("$3");
    register GeomState *offset_geometry;
    int flags = camera->flags;
    register int screen_x asm("$6");
    register int screen_y asm("$7");
    register int center_x asm("$6");
    register int center_y asm("$7");
    register int sum_x;
    register int sum_y;
    int view_x;
    register int view_y;
    register int half_width;
    int half_height;
    register int signed_height asm("$2");
    register int signed_width asm("$3");
    register int width_offset asm("$2");
    register unsigned raw_width;
    register unsigned raw_height asm("$3");
    register int target_x asm("$9");
    register int target_y;
    register int bound asm("$3");
    register int y_bound;
    register int selected_x;

    if (flags & 7)
        return -23;
    cx = 160;
    if (!(flags & 64))
        return -24;
    {
        int value = position[1];

        local.point[0] = value;
    }
    {
        int value = position[3];

        local.point[1] = value;
    }
    {
        int value = position[5];

        local.point[2] = value;
    }
    {
        unsigned int cy = 112;
        gte_SetGeomOffset(cx, cy);
    }
    {
        register u32 **address = &camera->position.matrixWords;
        register u32 *matrix asm("$11");
        asm("" : "=r"(address) : "0"(address));
        matrix = *address;
        gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
    }
    {
        register int distance = *D_800BCFA8;
        gte_ctc2_26(distance);
    }
    gte_lwc2_0_0(local.point);
    gte_lwc2_1_4(local.point);
    PE1_NOP();
    PE1_NOP();
    gte_rtps_command();
    gte_stsxy2(local.projected);
    screen_x = local.projected[0];
    screen_y = local.projected[1];
    if (camera->flags & 128) {
        D_800BCFB4 = screen_x;
        D_800BCFB6 = screen_y;
    }
    geometry = D_800B1624;
    sum_x = D_800BCFB4 + screen_x;
    center_x = sum_x / 2;
    D_800BCFB4 = center_x;
    sum_y = D_800BCFB6 + screen_y;
    sum_y += (unsigned)sum_y >> 31;
    view_x = 320 - center_x;

    offset_geometry = D_800B1624;

    center_y = sum_y >> 1;
    D_800BCFB6 = center_y;
    asm volatile("" : : : "memory");
    view = (CameraViewport *)((u8 *)geometry + offset_geometry->entry_offset_1C);
    view += g_GeomGroupSel;
    asm("" : : "r"(view) : "$3");
    height_center = 224;

    raw_width = view->width;
    view_y = height_center - center_y;
    raw_width <<= 16;
    signed_width = (int)raw_width >> 16;
    signed_width += raw_width >> 31;
    half_width = signed_width >> 1;
    width_offset = half_width - 160;

    raw_height = view->height;
    target_x = width_offset + center_x;
    raw_height <<= 16;
    signed_height = (int)raw_height >> 16;
    signed_height += raw_height >> 31;
    half_height = signed_height >> 1;
    target_y = half_height + (center_y - 112);
    bound = view->minX;
    if (target_x < bound)
        goto adjust_x;
    bound = view->maxX;
    if (bound < target_x)
        goto adjust_x;
    goto check_y;
adjust_x:
    correction = bound - 160;

    view_x = half_width - correction;
check_y:
    y_bound = view->minY;
    if (target_y < y_bound)
        goto adjust_y;
    y_bound = view->maxY;
    if (y_bound < target_y)
        goto adjust_y;
    goto store_offsets;
adjust_y:
    fallback_height = view->height;
    fallback_half = (s16)fallback_height / 2;
    correction = y_bound - 112;

    view_y = fallback_half - correction;
store_offsets:
    D_800BCF94 = view_x;
    D_800BCF96 = view_y;
    selected_x = target_x;
    if (D_800BCF88.flags & 64) {
        register int signed_x asm("$3");
        register int signed_y;
        register int loaded_bound asm("$2");
        register int chosen_bound asm("$5");
        register int upper_y;
        geometry = D_800B1624;
        view = (CameraViewport *)((u8 *)geometry + geometry->entry_offset_1C);
        view += g_GeomGroupSel;
        signed_x = (s16)target_x;
        loaded_bound = view->minX;
        chosen_bound = loaded_bound;
        asm("" : "=r"(chosen_bound) : "0"(chosen_bound));
        if (signed_x < loaded_bound) {
            signed_y = (unsigned)target_y << 16;
            D_800BCF8C.x = chosen_bound;
        } else {
            loaded_bound = view->maxX;
            chosen_bound = loaded_bound;
            asm("" : "=r"(chosen_bound) : "0"(chosen_bound));
            if (loaded_bound < signed_x) {
                signed_y = (unsigned)target_y << 16;
                D_800BCF8C.x = chosen_bound;
            } else {
                signed_y = (unsigned)target_y << 16;
                D_800BCF8C.x = selected_x;
            }
        }
        signed_y >>= 16;
        loaded_bound = view->minY;
        chosen_bound = loaded_bound;
        asm("" : "=r"(chosen_bound) : "0"(chosen_bound));
        if (signed_y < loaded_bound) {
            D_800BCF8E = chosen_bound;
            return 0;
        }
        loaded_bound = view->maxY;
        upper_y = loaded_bound;
        asm("" : "=r"(upper_y) : "0"(upper_y));
        if (loaded_bound < signed_y) {
            D_800BCF8E = upper_y;
            return 0;
        }
        D_800BCF8E = target_y;
    }
    return 0;
}
int Render_SetGteScreenOffset(void) {
    register int x asm("v1") = D_800BCF94;
    register int y asm("a0") = D_800BCF96;
    register int sx asm("t4");
    register int sy asm("t5");


    sx = x << 16;
    sy = y << 16;
    asm volatile("ctc2 %0,$24" : : "r"(sx));
    asm volatile("ctc2 %0,$25" : : "r"(sy));
    return 0;
}

int Render_ResetGteScreenOffset(void) {
    register int x asm("v1") = 0xA0;
    register int y asm("a0") = 0x70;
    register int sx asm("t4");
    register int sy asm("t5");

    asm volatile("" : "=r"(x), "=r"(y) : "0"(x), "1"(y));
    sx = x << 16;
    sy = y << 16;
    asm volatile("ctc2 %0,$24" : : "r"(sx));
    asm volatile("ctc2 %0,$25" : : "r"(sy));
    return 0;
}
