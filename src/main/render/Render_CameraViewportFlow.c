/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/geom_state.h"
#include "pe1/render_camera.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_nop.h"
#include "pe1/render_lighting.h"
#include "pe1/render_object.h"
#include "pe1/field_movement.h"
#include "pe1/battle_runtime.h"
#include "pe1/render_prim.h"

/* Resets the view state at the start of a room draw: render flags, the
 * scroll and camera words, the light colour, the mesh group and depth, and
 * points the view at the given matrix and projection distance. */
int Render_InitViewState(void *matrix, void *screen) {
    D_800BCF88.words.flags = 0x70;
    D_800BCFFC = 0x60;
    D_800BCFFE = 0x180;
    D_800BD027 = 0xFF;
    D_800BD026 = 0xFF;
    D_800BD025 = 0xFF;
    D_800BCF88.words.position = 0;
    D_800BCF88.words.saved = 0;
    D_800BCF88.words.screenOffset = 0;
    D_800BCF88.words.start = 0;
    D_800BCF88.words.target = 0;
    D_800BCF88.words.timer = 0;
    D_800BCF88.words.viewMatrix = matrix;
    D_800BCF88.words.projectionDistance = screen;
    D_800BCF88.words.savedBoundsX = 0;
    D_800BCF88.words.savedBoundsY = 0;
    D_800BCF88.words.cameraOffset = 0;
    g_GeomGroupSel = 0;
    D_800BD022 = 0;
    D_800BD020 = 0;
    D_800BD024 = 0;
    D_800BCF88.words.origin = 0;
    return 0;
}

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

int Render_SaveAndOpenBounds(void) {
    char *base = g_GeomStateBytes;
    CameraViewport *entry = (CameraViewport *)(g_GeomStateBytes + *(int *)(base + 0x1C)) + g_GeomGroupSel;

    D_800BCFAC = *(u16 *)&entry->minX;
    D_800BCFAE = *(u16 *)&entry->maxX;
    D_800BCFB0 = *(u16 *)&entry->minY;
    D_800BCFB2 = *(u16 *)&entry->maxY;

    *(short *)((u8 *)entry + 0x2C) = -0x8000;
    *(u16 *)&entry->maxX = 0x7FFF;
    *(short *)((u8 *)entry + 0x30) = -0x8000;
    *(u16 *)&entry->maxY = 0x7FFF;

    return 0;
}

int Render_RestoreBounds(void) {
    char *base = g_GeomStateBytes;
    CameraViewport *entry = (CameraViewport *)(g_GeomStateBytes + *(int *)(base + 0x1C)) + g_GeomGroupSel;

    *(u16 *)&entry->minX = D_800BCFAC;
    *(u16 *)&entry->maxX = D_800BCFAE;
    *(u16 *)&entry->minY = D_800BCFB0;
    *(u16 *)&entry->maxY = D_800BCFB2;

    return 0;
}
/* Matching debt: register pins, empty scheduling constraints, a $v1 clobber,
 * explicit signed-halving steps, gotos and 40 bytes of frame padding. */

extern int *D_800BCFA8;

int Render_SetViewport(s16 *position) {
    struct {
        s16 point[4];
        s16 projected[2];
        unsigned reserved[10];
    } local;
    GeomScrollState *camera = &D_800BCF88.state;
    register GeomState *geometry;
    register CameraViewport *view;
    register unsigned int cx asm("$11");
    register GeomState *offset_geometry;
    int flags = camera->flags;
    register int screen_x asm("$6");
    register int screen_y asm("$7");
    register int sum_x;
    register int sum_y;
    int view_x;
    register int view_y;
    register int half_width;
    int half_height;
    register int axisValue asm("$2");
    register int viewportValue asm("$3");
    register unsigned raw_width;
    register int target_x asm("$9");
    register int target_y;
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
        /* C offset shifts; pins and the empty constraint retain scheduling. */
        {
                        gte_SetGeomOffset(cx, cy);
        }
    }
    {
        register u32 **address = &camera->position.matrixWords;
        asm("" : "=r"(address) : "0"(address));
        {
            register const GteMatrixWords *words asm("$11");
            words = (const GteMatrixWords *)*address;
            gte_ldrotmatrix(words);
            gte_ldtransmatrix(words);
        }
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
    screen_x = sum_x / 2;
    D_800BCFB4 = screen_x;
    sum_y = D_800BCFB6 + screen_y;
    sum_y += (unsigned)sum_y >> 31;
    view_x = 320 - screen_x;

    offset_geometry = D_800B1624;

    screen_y = sum_y >> 1;
    D_800BCFB6 = screen_y;
    asm volatile("" : : : "memory");
    view = (CameraViewport *)((u8 *)geometry + offset_geometry->entry_offset_1C);
    view += g_GeomGroupSel;
    asm("" : : "r"(view) : "$3");
    viewportValue = 224;

    raw_width = view->width;
    view_y = viewportValue - screen_y;
    raw_width <<= 16;
    viewportValue = (int)raw_width >> 16;
    viewportValue += raw_width >> 31;
    half_width = viewportValue >> 1;
    axisValue = half_width - 160;

    viewportValue = view->height;
    target_x = axisValue + screen_x;
    viewportValue = (unsigned)viewportValue << 16;
    axisValue = viewportValue >> 16;
    axisValue += (unsigned)viewportValue >> 31;
    half_height = axisValue >> 1;
    target_y = half_height + (screen_y - 112);
    viewportValue = view->minX;
    if (target_x < viewportValue ||
        (viewportValue = view->maxX, viewportValue < target_x)) {
        axisValue = viewportValue - 160;
        view_x = half_width - axisValue;
    }
    y_bound = view->minY;
    if (target_y < y_bound ||
        (y_bound = view->maxY, y_bound < target_y)) {
        axisValue = view->height;
        viewportValue = (s16)axisValue / 2;
        axisValue = y_bound - 112;
        view_y = viewportValue - axisValue;
    }
    D_800BCF94 = view_x;
    D_800BCF96 = view_y;
    selected_x = target_x;
    if (D_800BCF88.state.flags & 64) {
        register int signed_y;
        s16 chosen_bound;
        register int upper_y;
        geometry = D_800B1624;
        view = (CameraViewport *)((u8 *)geometry + geometry->entry_offset_1C);
        view += g_GeomGroupSel;
        viewportValue = (s16)target_x;
        axisValue = view->minX;
        chosen_bound = axisValue;
        if (viewportValue < axisValue) {
            signed_y = (unsigned)target_y << 16;
            D_800BCF8C.x = chosen_bound;
        } else {
            axisValue = view->maxX;
            chosen_bound = axisValue;
            if (axisValue < viewportValue) {
                signed_y = (unsigned)target_y << 16;
                D_800BCF8C.x = chosen_bound;
            } else {
                signed_y = (unsigned)target_y << 16;
                D_800BCF8C.x = selected_x;
            }
        }
        signed_y >>= 16;
        axisValue = view->minY;
        chosen_bound = axisValue;
        if (signed_y < axisValue) {
            D_800BCF8E = chosen_bound;
            return 0;
        }
        axisValue = view->maxY;
        upper_y = axisValue;
        asm("" : "=r"(upper_y) : "0"(upper_y));
        if (axisValue < signed_y) {
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
    gte_ctc2_24(sx);
    gte_ctc2_25(sy);
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
    gte_ctc2_24(sx);
    gte_ctc2_25(sy);
    return 0;
}

extern struct { char _[16]; } D_800BCF88_o __asm__("D_800BCF88");
extern struct { char _[16]; } D_800BCF8C_o __asm__("D_800BCF8C");
extern struct { char _[16]; } D_800BCF98_o __asm__("D_800BCF98");
extern struct { char _[16]; } D_800BCF9C_o __asm__("D_800BCF9C");
extern struct { char _[16]; } D_800BCF9E_o __asm__("D_800BCF9E");
extern struct { char _[16]; } D_800BCFA0_o __asm__("D_800BCFA0");
extern struct { char _[16]; } D_800BCFA2_o __asm__("D_800BCFA2");

#define D_800BCF88 (*(int *)&D_800BCF88_o)
#define D_800BCF8C (*(int *)&D_800BCF8C_o)
#define D_800BCF98 (*(int *)&D_800BCF98_o)
#define D_800BCF9C (*(s16 *)&D_800BCF9C_o)
#define D_800BCF9E (*(s16 *)&D_800BCF9E_o)
#define D_800BCFA0 (*(s16 *)&D_800BCFA0_o)
#define D_800BCFA2 (*(s16 *)&D_800BCFA2_o)

int Render_SetScrollMode(int x, int y, int z, int mode) {
    int *flags_ptr;
    int flags;
    int value;
    int saved_scroll;
    flags_ptr = &D_800BCF88;
    flags = *flags_ptr;
    if ((flags & 0x40) == 0) {
        return -0x13;
    }

    value = 1;
    saved_scroll = D_800BCF8C;
    D_800BCFA0 = value;
    value = -0x10;
    D_800BCF9C = x;
    D_800BCF9E = y;
    D_800BCFA2 = z;
    D_800BCF98 = saved_scroll;

    saved_scroll = flags & value;
    value = 8;
    if (mode != value) {
        value = saved_scroll | 1;
    } else {
        value = saved_scroll | 9;
    }
    *flags_ptr = value;

    return 0;
}

#undef D_800BCF88
#undef D_800BCF8C
#undef D_800BCF98
#undef D_800BCF9C
#undef D_800BCF9E
#undef D_800BCFA0
#undef D_800BCFA2

/* Advance the camera transition, then center the selected viewport. */
int Scene_IsNotBattleMode(void)
{
    unsigned int flags = g_RenderStateFlags;
    int mode, smooth;
    u16 dx, dy, duration;
    if (!(flags & 0x40))
        return -20;
    mode = flags & 7;
    smooth = flags & 8;
    if (mode != 1 && mode != 2 && mode != 3)
        return 0;
    {
        u16 targetX = D_800BCF9C, x = D_800BCF98;
        u16 targetY = D_800BCF9E, y = D_800BCF9A;
        u16 time = D_800BCFA0;
        duration = D_800BCFA2;
        dx = targetX - x;
        dy = targetY - y;
        if (!smooth) {
            int movedX = (short)dx * (short)time / (short)duration;
            int movedY = (short)dy * (short)time / (short)duration;
            D_800BCF8C.x = x + movedX;
            D_800BCF8E = y + movedY;
        } else {
            int weight = rcos((short)time * 2048 / (short)duration + 2048) + 4096;
            int movedX = (short)dx * weight / 8192;
            int movedY = (short)dy * weight / 8192;
            D_800BCF8C.x = D_800BCF98 + movedX;
            D_800BCF8E = D_800BCF9A + movedY;
        }
    }
    {
        GeomState *state = D_800B1624;
        CameraViewport *views = (CameraViewport *)((u8 *)D_800B1624 + state->entry_offset_1C);
        CameraViewport *view = &views[g_GeomGroupSel];
        {
            unsigned int offset = 160 - (D_800BCF8C.x - (short)view->width / 2);
            D_800BCF94 = offset;
        }
        {
            unsigned int offset = 112 - ((short)D_800BCF8E - (short)view->height / 2);
            D_800BCF96 = offset;
        }
    }
    if (mode == 1)
        g_RenderStateFlags = (g_RenderStateFlags & ~7) | 2;
    {
        u16 time = D_800BCFA0 + 1;
        D_800BCFA0 = time;
        if ((short)time > (short)duration) {
            unsigned int completed = g_RenderStateFlags;
            if ((completed & 7) == 2)
                g_RenderStateFlags = (completed & ~7) | 0x84;
            else
                g_RenderStateFlags = (completed & ~7) | 0x80;
        }
    }
    return 0;
}

/* Project a world-space point and start a clamped camera transition. */
int Render_UpdateScrollPosition(void *positionArg, int duration, int mode)
{
    int *position = positionArg;
    short vector[4];
    short projected[2];
    GeomScrollState *camera = &D_800BCF88.state;
    CameraViewport *view;
    int x, y;

    if (!(camera->flags & 64))
        return -21;
    D_800BCF98 = D_800BCF8C.x;
    D_800BCF9A = D_800BCF8E;
    vector[0] = position[0]>>16;
    vector[1] = (position[1]>>16)-(u16)D_800BCFFE;
    vector[2] = position[2]>>16;
    {
        /* Narrow volatile register retains GCC's retail 24-byte frame.
         * This is a matching constraint, not evidence of the original type. */
        register volatile unsigned short cx asm("$10") = 160;
        unsigned int cy = 112;
        /* C offset shifts; pins and the empty constraint retain scheduling. */
        {
                        gte_SetGeomOffset(cx, cy);
        }
    }
    {
        unsigned int **address = &camera->position.matrixWords;
        asm("" : "=r"(address) : "0"(address));
        {
            register const GteMatrixWords *words asm("$10");
            words = (const GteMatrixWords *)*address;
            gte_ldrotmatrix(words);
            gte_ldtransmatrix(words);
        }
    }
    gte_lwc2_0_0(vector);
    gte_lwc2_1_4(vector);
    PE1_NOP();
    PE1_NOP();
    gte_rtps_command();
    gte_stsxy2(projected);
    {
        GeomState *state = D_800B1624;
        GeomStateAddress table;

        table.state = D_800B1624;
        table.word += state->entry_offset_1C;
        view = &table.viewport[g_GeomGroupSel];
    }
    {
        u16 width = view->width,height = view->height;
        int halfX = (short)width/2-160,halfY = (short)height/2-112;
        x = halfX+projected[0];
        y = halfY+projected[1];
    }
    {
        int min = view->minX;
        if (x < min) x = min;
        else {
            int max = view->maxX;
            if (x > max) x = max;
        }
    }
    {
        int min = view->minY;
        if (y < min) y = min;
        else {
            int max = view->maxY;
            if (y > max) y = max;
        }
    }
    D_800BCF9C = x;
    D_800BCF9E = y;
    if (duration == -1)
        D_800BCFA2 = 30;
    else
        D_800BCFA2 = duration;
    D_800BCFA0 = 1;
    if (mode == -1)
        g_RenderStateFlags = (g_RenderStateFlags & ~7) | 3;
    else {
        unsigned int flags = g_RenderStateFlags & ~15;
        unsigned int bits = mode | 3;
        g_RenderStateFlags = flags | bits;
    }
    return 0;
}

extern u8 g_GeomGroupSel;                  /* active mesh-entry GROUP selector: only
                                       * entries with entry[+0x24]==this are drawn
                                       * (one bg layer-set / camera view) */
extern u16 * volatile g_GeomVramPacketDst;

void SetGeomScreen(int h);

int Gpu_LoadGeomState(int index) {
    GeomStateAddress table, base;
    CameraViewport *entry;
    u16 *dst;
    int value;

    GEOM_STATE_OFFSET(table, base, entry_offset_1C, index * 52);
    entry = table.viewport;
    *D_800BCFA8 = entry->prefix.gpu.geom_screen;
    SetGeomScreen(entry->prefix.gpu.geom_screen);

    dst = g_GeomVramPacketDst;
    dst[0] = entry->prefix.gpu.half02;
    dst = g_GeomVramPacketDst;
    dst[1] = entry->prefix.gpu.half04;
    dst = g_GeomVramPacketDst;
    dst[2] = entry->prefix.gpu.half06;
    dst = g_GeomVramPacketDst;
    dst[3] = entry->prefix.gpu.half08;
    dst = g_GeomVramPacketDst;
    dst[4] = entry->prefix.gpu.half0A;
    dst = g_GeomVramPacketDst;
    dst[5] = entry->prefix.gpu.half0C;
    dst = g_GeomVramPacketDst;
    dst[6] = entry->prefix.gpu.half0E;
    dst = g_GeomVramPacketDst;
    dst[7] = entry->prefix.gpu.half10;
    dst = g_GeomVramPacketDst;
    dst[8] = entry->prefix.gpu.half12;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[5] = entry->prefix.gpu.word14;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[6] = entry->prefix.gpu.word18;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[7] = entry->prefix.gpu.word1C;

    g_GeomGroupSel = index;
    value = g_RenderStateFlags;
    g_RenderStateFlags = value | 0x80;
    return 0;
}

static inline int ClampLight(int value)
{
    if (value < 0) return 0;
    if (value > 255) return 255;
    return value;
}

/* Build two opposite Y lights and upload their color matrix to the GTE. */
int Render_InitRoomPrimState(void *objectArg)
{
    RenderObjectEntity *object = objectArg;
    int intensity, r, g, b;

    D_800BEA40.matrix.m[0][0] = 0;
    D_800BEA42 = 4096;
    D_800BEA44 = 0;
    intensity = object->shade + object->lightPositiveY;
    r = D_800BD025;
    g = D_800BD026;
    b = D_800BD027;
    intensity = ClampLight(intensity) << 4;
    D_800BEA60.matrix.m[0][0] = intensity * r / 256;
    D_800BEA66 = intensity * g / 256;
    D_800BEA6C = intensity * b / 256;

    D_800BEA46 = 0;
    D_800BEA48 = -4096;
    D_800BEA4A = 0;
    intensity = object->shade + object->lightNegativeY;
    intensity = ClampLight(intensity) << 4;
    D_800BEA62 = intensity * r / 256;
    D_800BEA68 = intensity * g / 256;
    {
        int result = intensity * b / 256;
        asm("" : : "r"(result));
        D_800BEA6E = result;
    }
    D_800BEA4C = 0;
    D_800BEA4E = 0;
    D_800BEA50 = 0;
    D_800BEA64 = 0;
    D_800BEA6A = 0;
    D_800BEA70 = 0;
    {
        register u32 *matrix asm("$9") = D_800BEA60.words;
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        /* Commit the overlapping halfword aliases before reading words. */
        asm("" : "=r"(matrix) : "0"(matrix) : "memory");
        a = matrix[0];
        b = matrix[1];
        gte_ctc2_16(a);
        gte_ctc2_17(b);
        a = matrix[2];
        b = matrix[3];
        c = matrix[4];
        gte_ctc2_18(a);
        gte_ctc2_19(b);
        gte_ctc2_20(c);
    }
    return 0;
}

extern unsigned short g_ScreenTransitionTargetY;
extern unsigned char g_ScreenTransitionState;
extern unsigned char g_ScreenTransitionMode;
extern unsigned short g_ScreenTransitionStartY;
extern unsigned short g_ScreenTransitionFadeColor;

int CdRom_SetSeekPos(unsigned int arg0) {
    unsigned short old0;
    unsigned short old1;
    unsigned short old2;

    old0 = (u16)D_800BCFE8;
    old1 = g_ScreenTransitionTargetY;
    old2 = (u16)D_800BCFEC;

    D_800BCFE8 = 0xFF;
    g_ScreenTransitionTargetY = 0xFF;
    D_800BCFEC = 0xFF;
    g_ScreenTransitionState = 2;
    g_ScreenTransitionMode = 2;
    g_ScreenTransitionFadeColor = arg0;
    D_800BCFF8 = 0;
    D_800BCFF0 = old0;
    g_ScreenTransitionStartY = old1;
    D_800BCFF4 = old2;

    return 0;
}

int CdRom_SetScreenPos(int arg0, int arg1, int arg2, int arg3, unsigned short arg4) {
    unsigned short old0;
    unsigned short old1;
    unsigned short old2;
    int mode;

    old0 = (u16)D_800BCFE8;
    old1 = g_ScreenTransitionTargetY;
    old2 = (u16)D_800BCFEC;

    D_800BCFE8 = arg1;
    g_ScreenTransitionTargetY = arg2;
    D_800BCFEC = arg3;
    g_ScreenTransitionState = 2;
    g_ScreenTransitionFadeColor = arg0;
    D_800BCFF8 = 0;
    D_800BCFF0 = old0;
    mode = arg4;
    g_ScreenTransitionStartY = old1;
    D_800BCFF4 = old2;

    if (mode >= 4) {
        goto fail;
    }
    if (mode < 0) {
        goto fail;
    }

    g_ScreenTransitionMode = arg4;
    goto done;

fail:
    g_ScreenTransitionMode = 1;

done:
    return 0;
}

int Render_SetFadeColour(unsigned int arg0) {
    unsigned short old0;
    unsigned short old1;
    unsigned short old2;

    old0 = (u16)D_800BCFE8;
    old1 = g_ScreenTransitionTargetY;
    old2 = (u16)D_800BCFEC;

    g_ScreenTransitionState = 6;
    D_800BCFE8 = 0;
    g_ScreenTransitionTargetY = 0;
    D_800BCFEC = 0;
    g_ScreenTransitionFadeColor = arg0;
    D_800BCFF8 = 0;
    D_800BCFF0 = old0;
    g_ScreenTransitionStartY = old1;
    D_800BCFF4 = old2;

    return 0;
}

/* Build the camera basis from yaw using the retail GTE operations.
 * Matching debt: 4 register pins and 5 empty constraints. Matrix/vector
 * loads are C, with GTE transfers and commands wrapped individually. */
int Render_DrawSprite(void)
{
    GteMatrixStorage rotation;
    GteShortVector axis;
    GteVector up,right,newUp,forward;
    int angle,cosine,sine;
    if((D_800BE9A0&0xF000)==0x7000)angle = D_800BD022;
    else angle = D_800BD020;
    {
        int masked = angle&4095;
        if(D_8009D254 && (D_8009D2E8&16)) {
            angle+=2048;
            angle+=(((unsigned int)((Combatant *)D_8009D254->core)->stateFlags)>>7)&0xC00;
            masked = angle&4095;
        }
        cosine = rcos(masked);
        sine = rsin(masked);
    }
    rotation.matrix.m[0][2] = sine;
    rotation.matrix.m[2][0] = -sine;
    rotation.matrix.m[1][1] = 4096;
    axis.z = 4096;
        rotation.matrix.m[0][0] = cosine;
    rotation.matrix.m[2][2] = cosine;
    rotation.matrix.t[0] = rotation.matrix.t[1] = rotation.matrix.t[2] = 0;
    rotation.matrix.m[0][1] = rotation.matrix.m[1][0] = rotation.matrix.m[1][2] = rotation.matrix.m[2][1] = 0;
    axis.x = 0;
    axis.y = 0;
    {
        const GteMatrixWords *matrix;
        matrix = (const GteMatrixWords *)rotation.words;
                gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
    }
    gte_lwc2_0_0(&axis);
    gte_lwc2_1_4(&axis);
    PE1_NOP();
    PE1_NOP();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&forward);
    gte_swc2_26_4(&forward);
    gte_swc2_27_8(&forward);
    {
        GteVector *source;
        up.y = 4096;
                source = &up;
        up.x = 0;
        up.z = 0;
                gte_ldopv1_psyq(source);
    }
    {
        gte_ldopv2(&forward);
        gte_op12_psyq();
        gte_swc2_25_0(&right);
        gte_swc2_26_4(&right);
        gte_swc2_27_8(&right);
    }
    {
        GteVector *source = &forward;
                gte_ldopv1_psyq(source);
    }
    {
        gte_ldopv2(&right);
        gte_op12_psyq();
        gte_swc2_25_0(&newUp);
        gte_swc2_26_4(&newUp);
        gte_swc2_27_8(&newUp);
    }
    {
        int xx = right.x,xy = newUp.x,xz = forward.x;
        int yx = right.y,yy = newUp.y,yz = forward.y;
        int zx = right.z,zy = newUp.z,zz = forward.z;
        D_800BD014 = 0;
        D_800BD018 = 0;
        D_800BD01C = 0;
        D_800BD000.m[0][0] = xx;
        D_800BD002 = xy;
        D_800BD004 = xz;
        D_800BD006 = yx;
        D_800BD008 = yy;
        D_800BD00A = yz;
        D_800BD00C = zx;
        D_800BD00E = zy;
        D_800BD010 = zz;
    }
    return 0;
}
