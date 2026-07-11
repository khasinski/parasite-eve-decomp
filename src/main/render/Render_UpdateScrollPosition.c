typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
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
extern s16 D_800BCF98;
extern s16 D_800BCF9A;
extern s16 D_800BCF9C;
extern s16 D_800BCF9E;
extern s16 D_800BCFA0;
extern s16 D_800BCFA2;
extern int *D_800BCFA4;
extern s16 g_CameraBaseAngleY;
extern u8 g_GeomGroupSel;
extern GeomState *g_GeomState;

int RotTransPers(void *v, void *sxy, void *p, int *flag);

static void Render_SetScreenOffsetRegs(int x, int y) {
    x <<= 16;
    y <<= 16;
    asm volatile("ctc2 %0,$24" : : "r"(x));
    asm volatile("ctc2 %0,$25" : : "r"(y));
}

static void Render_LoadScrollMatrix(int *matrix) {
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

static s32 Render_HalfTowardZero(s32 value) {
    if (value < 0) {
        value++;
    }
    return value >> 1;
}

static s32 Render_ClampToRange(s32 value, s32 min, s32 max) {
    if (value < min) {
        return min;
    }
    if (max < value) {
        return max;
    }
    return value;
}

static CameraBounds *Render_GetCameraBounds(void) {
    GeomState *state;
    CameraBounds *bounds;

    state = g_GeomState;
    bounds = (CameraBounds *)((u8 *)state + state->bounds_offset);
    return &bounds[g_GeomGroupSel];
}

s32 Render_UpdateScrollPosition(s16 *pos, s32 duration, s32 mode) {
    Vec3s point;
    s32 sxy;
    s32 p;
    s32 flag;
    s32 target_x;
    s32 target_y;
    CameraBounds *bounds;

    if ((D_800BCF88 & 0x40) == 0) {
        return -0x15;
    }

    D_800BCF98 = D_800BCF8C;
    D_800BCF9A = g_CameraClampedY;

    point.x = pos[1];
    point.y = pos[3] - g_CameraBaseAngleY;
    point.z = pos[5];
    point.pad = 0;

    Render_SetScreenOffsetRegs(0xA0, 0x70);
    Render_LoadScrollMatrix(D_800BCFA4);
    RotTransPers(&point, &sxy, &p, &flag);

    bounds = Render_GetCameraBounds();
    target_x = Render_HalfTowardZero(bounds->width) + (s16)sxy - 0xA0;
    target_y = Render_HalfTowardZero(bounds->height) + (s16)(sxy >> 16) - 0x70;

    D_800BCF9C = Render_ClampToRange(target_x, bounds->min_x, bounds->max_x);
    D_800BCF9E = Render_ClampToRange(target_y, bounds->min_y, bounds->max_y);

    if (duration == -1) {
        D_800BCFA2 = 0x1E;
    } else {
        D_800BCFA2 = duration;
    }
    D_800BCFA0 = 1;

    if (mode == -1) {
        D_800BCF88 = (D_800BCF88 & -8) | 3;
    } else {
        D_800BCF88 = (D_800BCF88 & -16) | (mode | 3);
    }

    return 0;
}
