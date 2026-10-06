/* MASPSX_FLAGS: --expand-div */
#include "pe1/geom_state.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_nop.h"
#include "pe1/render_lighting.h"
#include "pe1/render_object.h"
#include "pe1/render_camera.h"
#include "pe1/field_movement.h"
#include "pe1/battle_runtime.h"

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
    GeomScrollState *camera = &D_800BCF88;
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
        register unsigned int cy asm("$11") = 112;
        /* C offset shifts; pins and the empty constraint retain scheduling. */
        {
            register u32 ofx asm("$12");
            register u32 ofy asm("$13");
            asm volatile("" : "=r"(cx), "=r"(cy) : "0"(cx), "1"(cy));
            ofx = (u32)cx << 16;
            ofy = cy << 16;
            gte_ctc2_24(ofx);
            gte_ctc2_25(ofy);
        }
    }
    {
        unsigned int **address = &camera->position.matrixWords;
        asm("" : "=r"(address) : "0"(address));
        {
            register const GteMatrixWords *words asm("$10");
            register u32 a asm("$12");
            register u32 b asm("$13");
            register u32 c asm("$14");
            words = (const GteMatrixWords *)*address;
            a = words->r11_r12;
            b = words->r13_r21;
            gte_ctc2_0(a);
            gte_ctc2_1(b);
            a = words->r22_r23;
            b = words->r31_r32;
            c = words->r33_pad;
            gte_ctc2_2(a);
            gte_ctc2_3(b);
            gte_ctc2_4(c);
            a = words->tx;
            b = words->ty;
            gte_ctc2_5(a);
            c = words->tz;
            gte_ctc2_6(b);
            gte_ctc2_7(c);
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

extern int g_RenderStateFlags;
extern u8 g_GeomGroupSel;                  /* active mesh-entry GROUP selector: only
                                       * entries with entry[+0x24]==this are drawn
                                       * (one bg layer-set / camera view) */
extern int *D_800BCFA8;
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
    register u32 a asm("$12");
    register u32 b asm("$13");
    register u32 c asm("$14");
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
    asm volatile("" : : : "memory");
    rotation.matrix.m[0][0] = cosine;
    rotation.matrix.m[2][2] = cosine;
    rotation.matrix.t[0] = rotation.matrix.t[1] = rotation.matrix.t[2] = 0;
    rotation.matrix.m[0][1] = rotation.matrix.m[1][0] = rotation.matrix.m[1][2] = rotation.matrix.m[2][1] = 0;
    axis.x = 0;
    axis.y = 0;
    {
        const GteMatrixWords *matrix;
        matrix = (const GteMatrixWords *)rotation.words;
        asm volatile("" : "=r"(matrix) : "0"(matrix));
        a = matrix->r11_r12;
        b = matrix->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = matrix->r22_r23;
        b = matrix->r31_r32;
        c = matrix->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = matrix->tx;
        b = matrix->ty;
        gte_ctc2_5(a);
        c = matrix->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
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
        asm volatile("" : : : "memory");
        source = &up;
        up.x = 0;
        up.z = 0;
        asm volatile("" : "=r"(source) : "0"(source));
        a = source->x;
        b = source->y;
        gte_ctc2_0(a);
        c = source->z;
        gte_ctc2_2(b);
        gte_ctc2_4(c);
    }
    {
        gte_lwc2_11_8(&forward);
        gte_lwc2_9_0(&forward);
        gte_lwc2_10_4(&forward);
        PE1_NOP();
        PE1_NOP();
        gte_op_sf12_command();
        gte_swc2_25_0(&right);
        gte_swc2_26_4(&right);
        gte_swc2_27_8(&right);
    }
    {
        register GteVector *source asm("$3") = &forward;
        asm volatile("" : "=r"(source) : "0"(source));
        a = source->x;
        b = source->y;
        gte_ctc2_0(a);
        c = source->z;
        gte_ctc2_2(b);
        gte_ctc2_4(c);
    }
    {
        gte_lwc2_11_8(&right);
        gte_lwc2_9_0(&right);
        gte_lwc2_10_4(&right);
        PE1_NOP();
        PE1_NOP();
        gte_op_sf12_command();
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
