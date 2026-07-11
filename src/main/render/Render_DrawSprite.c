typedef signed short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

typedef struct GteMatrixWords {
    s32 w[8];
} GteMatrixWords;

typedef struct Vec3s {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} Vec3s;

typedef struct Vec3i {
    s32 x;
    s32 y;
    s32 z;
} Vec3i;

extern u16 D_800BCE9C;
extern u16 D_800BD020;
extern u16 D_800BD022;
extern void *D_8009D254;
extern u32 D_8009D2E8;

extern s16 D_800BD000;
extern s16 D_800BD002;
extern s16 D_800BD004;
extern s16 D_800BD006;
extern s16 D_800BD008;
extern s16 D_800BD00A;
extern s16 D_800BD00C;
extern s16 D_800BD00E;
extern s16 D_800BD010;
extern s32 D_800BD014;
extern s32 D_800BD018;
extern s32 D_800BD01C;

s32 rsin(s32 angle);
s32 rcos(s32 angle);

static void Render_LoadSpriteMatrix(GteMatrixWords *matrix) {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;

    w0 = matrix->w[0];
    w1 = matrix->w[1];
    w2 = matrix->w[2];
    w3 = matrix->w[3];
    w4 = matrix->w[4];
    w5 = matrix->w[5];
    w6 = matrix->w[6];
    w7 = matrix->w[7];

    asm volatile("ctc2 %0,$0" : : "r"(w0));
    asm volatile("ctc2 %0,$1" : : "r"(w1));
    asm volatile("ctc2 %0,$2" : : "r"(w2));
    asm volatile("ctc2 %0,$3" : : "r"(w3));
    asm volatile("ctc2 %0,$4" : : "r"(w4));
    asm volatile("ctc2 %0,$5" : : "r"(w5));
    asm volatile("ctc2 %0,$6" : : "r"(w6));
    asm volatile("ctc2 %0,$7" : : "r"(w7));
}

static void Render_TransformSpriteVec(Vec3s *vec, Vec3i *out) {
    asm volatile("lwc2 $0,0(%0)" : : "r"(vec));
    asm volatile("lwc2 $1,4(%0)" : : "r"(vec));
    asm volatile("nop");
    asm volatile(".word 0x4A480012");
    asm volatile("swc2 $25,0(%0)" : : "r"(out));
    asm volatile("swc2 $26,4(%0)" : : "r"(out));
    asm volatile("swc2 $27,8(%0)" : : "r"(out));
}

static void Render_LoadSpriteAxisMatrix(Vec3i *axis) {
    asm volatile("ctc2 %0,$0" : : "r"(axis->x));
    asm volatile("ctc2 %0,$2" : : "r"(axis->y));
    asm volatile("ctc2 %0,$4" : : "r"(axis->z));
}

static void Render_ProjectSpriteAxis(Vec3i *matrix_axis, Vec3i *vec, Vec3i *out) {
    Render_LoadSpriteAxisMatrix(matrix_axis);
    asm volatile("lwc2 $11,8(%0)" : : "r"(vec));
    asm volatile("lwc2 $9,0(%0)" : : "r"(vec));
    asm volatile("lwc2 $10,4(%0)" : : "r"(vec));
    asm volatile("nop");
    asm volatile(".word 0x4B78000C");
    asm volatile("swc2 $25,0(%0)" : : "r"(out));
    asm volatile("swc2 $26,4(%0)" : : "r"(out));
    asm volatile("swc2 $27,8(%0)" : : "r"(out));
}

static s32 Render_GetSpriteAngle(void) {
    s32 angle;

    if ((D_800BCE9C & 0xF000) == 0x7000) {
        angle = D_800BD022;
    } else {
        angle = D_800BD020;
    }

    if (D_8009D254 != 0 && (D_8009D2E8 & 0x10) != 0) {
        angle += 0x800;
        angle += ((*(s32 *)(*(s32 *)D_8009D254 + 0x4C) >> 7) & 0xC00);
    }

    return angle & 0xFFF;
}

s32 Render_DrawSprite(void) {
    GteMatrixWords rot;
    Vec3s seed;
    Vec3i axis_a;
    Vec3i axis_b;
    Vec3i axis_c;
    Vec3i scale_axis;
    s32 angle;
    s32 s;
    s32 c;

    angle = Render_GetSpriteAngle();
    s = rsin(angle);
    c = rcos(angle);

    rot.w[0] = (u16)s | (c << 16);
    rot.w[1] = 0x1000;
    rot.w[2] = (u16)-c;
    rot.w[3] = s;
    rot.w[4] = 0;
    rot.w[5] = 0;
    rot.w[6] = 0;
    rot.w[7] = 0;

    seed.x = 0;
    seed.y = 0;
    seed.z = 0x1000;
    seed.pad = 0;

    Render_LoadSpriteMatrix(&rot);
    Render_TransformSpriteVec(&seed, &axis_a);

    scale_axis.x = 0;
    scale_axis.y = 0x1000;
    scale_axis.z = 0;
    Render_ProjectSpriteAxis(&scale_axis, &axis_a, &axis_b);
    Render_ProjectSpriteAxis(&axis_a, &axis_b, &axis_c);

    D_800BD014 = 0;
    D_800BD018 = 0;
    D_800BD01C = 0;
    D_800BD000 = axis_b.x;
    D_800BD002 = axis_c.x;
    D_800BD004 = axis_a.x;
    D_800BD006 = axis_b.y;
    D_800BD008 = axis_c.y;
    D_800BD00A = axis_a.y;
    D_800BD00C = axis_b.z;
    D_800BD00E = axis_c.z;
    D_800BD010 = axis_a.z;

    return 0;
}
