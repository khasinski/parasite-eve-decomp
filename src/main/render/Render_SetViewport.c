typedef signed short s16;
typedef unsigned char u8;
typedef int s32;

typedef struct Vec3s {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} Vec3s;

typedef struct GeomState {
    char pad[0x1C];
    s32 bounds_offset;
} GeomState;

typedef struct CameraBounds {
    char pad[0x28];
    s16 width;
    s16 height;
    s16 min_x;
    s16 max_x;
    s16 min_y;
    s16 max_y;
} CameraBounds;

extern s32 D_800BCF88;
extern s16 D_800BCF8C;
extern s16 g_CameraClampedY;
extern s16 D_800BCF94;
extern s16 D_800BCF96;
extern int *D_800BCFA4;
extern int *D_800BCFA8;
extern s16 D_800BCFB4;
extern s16 D_800BCFB6;
extern u8 g_GeomGroupSel;
extern GeomState *g_GeomState;

int RotTransPers(void *v, void *sxy, void *p, int *flag);

static void Render_SetViewportScreenOffsetRegs(int x, int y) {
    x <<= 16;
    y <<= 16;
    asm volatile("ctc2 %0,$24" : : "r"(x));
    asm volatile("ctc2 %0,$25" : : "r"(y));
}

static void Render_SetViewportMatrix(int *matrix) {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;
    int w5;
    int w6;
    int w7;

    w0 = matrix[0];
    w1 = matrix[1];
    w2 = matrix[2];
    w3 = matrix[3];
    w4 = matrix[4];
    w5 = matrix[5];
    w6 = matrix[6];
    w7 = matrix[7];

    asm volatile("ctc2 %0,$0" : : "r"(w0));
    asm volatile("ctc2 %0,$1" : : "r"(w1));
    asm volatile("ctc2 %0,$2" : : "r"(w2));
    asm volatile("ctc2 %0,$3" : : "r"(w3));
    asm volatile("ctc2 %0,$4" : : "r"(w4));
    asm volatile("ctc2 %0,$5" : : "r"(w5));
    asm volatile("ctc2 %0,$6" : : "r"(w6));
    asm volatile("ctc2 %0,$7" : : "r"(w7));
}

static void Render_SetViewportGeomScreen(void) {
    int geom_screen;

    geom_screen = *D_800BCFA8;
    asm volatile("ctc2 %0,$26" : : "r"(geom_screen));
}

static s32 Render_SetViewportHalfTowardZero(s32 value) {
    if (value < 0) {
        value++;
    }
    return value >> 1;
}

static s32 Render_SetViewportClamp(s32 value, s32 min, s32 max) {
    if (value < min) {
        return min;
    }
    if (max < value) {
        return max;
    }
    return value;
}

static CameraBounds *Render_SetViewportGetBounds(void) {
    GeomState *state;
    CameraBounds *bounds;

    state = g_GeomState;
    bounds = (CameraBounds *)((u8 *)state + state->bounds_offset);
    return &bounds[g_GeomGroupSel];
}

s32 Render_SetViewport(s16 *pos) {
    Vec3s point;
    s32 sxy;
    s32 p;
    s32 flag;
    s32 screen_x;
    s32 screen_y;
    s32 center_x;
    s32 center_y;
    s32 view_x;
    s32 view_y;
    s32 half_width;
    s32 half_height;
    s32 target_x;
    s32 target_y;
    CameraBounds *bounds;

    if (D_800BCF88 & 7) {
        return -0x17;
    }
    if ((D_800BCF88 & 0x40) == 0) {
        return -0x18;
    }

    point.x = pos[1];
    point.y = pos[3];
    point.z = pos[5];
    point.pad = 0;

    Render_SetViewportScreenOffsetRegs(0xA0, 0x70);
    Render_SetViewportMatrix(D_800BCFA4);
    Render_SetViewportGeomScreen();
    RotTransPers(&point, &sxy, &p, &flag);

    screen_x = (s16)sxy;
    screen_y = (s16)(sxy >> 16);

    if (D_800BCF88 & 0x80) {
        D_800BCFB4 = screen_x;
        D_800BCFB6 = screen_y;
    }

    center_x = Render_SetViewportHalfTowardZero(D_800BCFB4 + screen_x);
    center_y = Render_SetViewportHalfTowardZero(D_800BCFB6 + screen_y);
    D_800BCFB4 = center_x;
    D_800BCFB6 = center_y;

    view_x = 0x140 - center_x;
    view_y = 0xE0 - center_y;

    bounds = Render_SetViewportGetBounds();
    half_width = Render_SetViewportHalfTowardZero((s16)bounds->width);
    half_height = Render_SetViewportHalfTowardZero((s16)bounds->height);

    target_x = half_width + center_x - 0xA0;
    target_y = half_height + center_y - 0x70;

    if (target_x < bounds->min_x) {
        view_x = half_width + 0xA0 - bounds->min_x;
    } else if (bounds->max_x < target_x) {
        view_x = half_width + 0xA0 - bounds->max_x;
    }

    if (target_y < bounds->min_y) {
        view_y = half_height + 0x70 - bounds->min_y;
    } else if (bounds->max_y < target_y) {
        view_y = half_height + 0x70 - bounds->max_y;
    }

    D_800BCF94 = view_x;
    D_800BCF96 = view_y;

    D_800BCF8C = Render_SetViewportClamp(target_x, bounds->min_x, bounds->max_x);
    g_CameraClampedY = Render_SetViewportClamp(target_y, bounds->min_y, bounds->max_y);

    return 0;
}
