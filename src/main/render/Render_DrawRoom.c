typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

typedef struct RoomVec {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} RoomVec;

extern s16 D_800942EC;
extern s32 D_8009CDDC;
extern u32 *D_800B0E38[];
extern s32 D_800A636C[];
extern s32 D_800C89F8[];
extern s32 D_800C8A18;

void Gte_NormalizeVec(void *src, void *dst);
void SetRotMatrix(s32 *matrix);
void SetTransMatrix(s32 *matrix);
void SetGeomScreen(s32 value);
int RotTransPers(void *v, void *sxy, void *p, int *flag);

static void Render_DrawRoomLoadMatrix(s32 *matrix) {
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

static void Render_DrawRoomLoadTrans(s32 *translation) {
    int x;
    int y;
    int z;

    x = translation[0];
    y = translation[1];
    z = translation[2];

    asm volatile("ctc2 %0,$5" : : "r"(x));
    asm volatile("ctc2 %0,$6" : : "r"(y));
    asm volatile("ctc2 %0,$7" : : "r"(z));
}

static void Render_DrawRoomTransformVec(void *src, s32 *dst) {
    asm volatile("lwc2 $0,0(%0)\n\t"
                 "lwc2 $1,4(%0)\n\t"
                 "nop\n\t"
                 "nop\n\t"
                 ".word 0x4A480012\n\t"
                 "swc2 $25,0(%1)\n\t"
                 "swc2 $26,4(%1)\n\t"
                 "swc2 $27,8(%1)"
                 :
                 : "r"(src), "r"(dst)
                 : "memory");
}

static void Render_DrawRoomTransformAxis(u16 *src, s16 *dst) {
    int x;
    int y;
    int z;

    asm volatile("mtc2 %0,$9" : : "r"(src[0]));
    asm volatile("mtc2 %0,$10" : : "r"(src[3]));
    asm volatile("mtc2 %0,$11" : : "r"(src[6]));
    asm volatile("nop");
    asm volatile(".word 0x4A49E012");
    asm volatile("mfc2 %0,$9" : "=r"(x));
    asm volatile("mfc2 %0,$10" : "=r"(y));
    asm volatile("mfc2 %0,$11" : "=r"(z));

    dst[0] = x;
    dst[3] = y;
    dst[6] = z;
}

static void Render_DrawRoomBuildRotMatrix(s32 *matrix, s16 *basis, s16 *out) {
    SetRotMatrix(matrix);
    Render_DrawRoomTransformAxis((u16 *)basis, out);
    Render_DrawRoomTransformAxis((u16 *)basis + 1, out + 1);
    Render_DrawRoomTransformAxis((u16 *)basis + 2, out + 2);
}

static void Render_DrawRoomBuildNormalMatrix(s16 *axis, s16 *out) {
    s32 tmp[8];

    tmp[0] = axis[0];
    tmp[1] = axis[3];
    tmp[2] = axis[6];
    tmp[3] = 0;
    tmp[4] = 0x1000;
    tmp[5] = 0;
    tmp[6] = 0;
    tmp[7] = 0;
    Render_DrawRoomLoadMatrix(tmp);
    Render_DrawRoomTransformAxis((u16 *)tmp, out);
}

static void Render_DrawRoomLinkPrim(u8 *prim, s32 depth) {
    u32 *ot;
    u32 old;
    u32 prim_word;

    if (depth >= 0x1000) {
        return;
    }

    ot = D_800B0E38[D_8009CDDC] + depth;
    old = *ot;
    prim_word = U32_AT(prim, 0);
    U32_AT(prim, 0) = (prim_word & 0xFF000000) | (old & 0x00FFFFFF);
    *ot = (old & 0xFF000000) | ((u32)prim & 0x00FFFFFF);
}

static s32 Render_DrawRoomProject(RoomVec *point, u8 *prim, int prim_off) {
    s32 sxy;
    s32 p;
    s32 flag;
    s32 depth;

    depth = RotTransPers(point, &sxy, &p, &flag);
    U32_AT(prim, prim_off) = sxy;
    return depth;
}

static s32 Render_DrawRoomAverageShade(u8 *actor) {
    int sum;
    int avg;

    if ((U16_AT(actor, 0x250) & 6) != 0) {
        sum = U8_AT(actor, 0x248) + U8_AT(actor, 0x249) + U8_AT(actor, 0x24A);
        return sum / 3;
    }

    avg = 0x80 * U8_AT(actor, 0x27D);
    if (avg < 0) {
        avg += 0x7F;
    }
    avg >>= 7;
    if (avg < 0) {
        avg++;
    }
    return avg >> 1;
}

static void Render_DrawRoomSetupPrim(u8 *actor, u8 *prim, s16 radius) {
    int shade;

    U8_AT(prim, 3) = 9;
    U8_AT(prim, 7) = 0x2C;

    shade = Render_DrawRoomAverageShade(actor);
    U8_AT(prim, 4) = shade;
    U8_AT(prim, 5) = shade;
    U8_AT(prim, 6) = shade;

    U8_AT(prim, 0x0D) = 0x40;
    U8_AT(prim, 0x15) = 0x40;
    U8_AT(prim, 0x1D) = 0x7F;
    U8_AT(prim, 0x25) = 0x7F;
    U8_AT(prim, 0x14) = 0x3F;
    U8_AT(prim, 0x24) = 0x3F;
    U16_AT(prim, 0x0E) = 0x7210;
    U16_AT(prim, 0x16) = 0x00CB;
    U8_AT(prim, 0x0C) = 0;
    U8_AT(prim, 0x1C) = 0;
    U8_AT(prim, 7) |= 2;

    (void)radius;
}

static void Render_DrawRoomDrawBillboard(u8 *actor, RoomVec *base, s16 radius) {
    u8 *prim;
    RoomVec point;
    s32 max_depth;
    s32 depth;
    int slot_offset;

    slot_offset = D_8009CDDC * 0x28;
    prim = PTR_AT(actor, 0x278) + slot_offset;
    Render_DrawRoomSetupPrim(actor, prim, radius);

    max_depth = -1;

    point = *base;
    point.x = -radius;
    point.y = 0;
    point.z = radius;
    depth = Render_DrawRoomProject(&point, prim, 8);
    if (depth >= 0) {
        max_depth = depth;
    }

    point.x = radius;
    point.z = radius;
    depth = Render_DrawRoomProject(&point, prim, 0x10);
    if (max_depth < depth) {
        max_depth = depth;
    }

    point.x = -radius;
    point.z = -radius;
    depth = Render_DrawRoomProject(&point, prim, 0x18);
    if (max_depth < depth) {
        max_depth = depth;
    }

    point.x = radius;
    point.z = -radius;
    depth = Render_DrawRoomProject(&point, prim, 0x20);
    if (max_depth < depth) {
        max_depth = depth;
    }

    Render_DrawRoomLinkPrim(prim, max_depth);
}

static void Render_DrawRoomBuildBasis(u8 *actor, s32 *matrix, s16 *basis, s16 *out_basis, s32 *translation) {
    s32 axis[3];
    s32 normalized[3];
    s16 tmp_basis[9];

    Render_DrawRoomLoadMatrix(matrix);
    Render_DrawRoomTransformVec(basis + 4, axis);
    if (axis[0] == 0 && axis[2] == 0) {
        normalized[0] = 0;
        normalized[1] = 0;
        normalized[2] = 0x1000;
    } else {
        Gte_NormalizeVec(axis, normalized);
    }

    tmp_basis[0] = normalized[0];
    tmp_basis[1] = 0;
    tmp_basis[2] = 0;
    tmp_basis[3] = 0;
    tmp_basis[4] = 0x1000;
    tmp_basis[5] = 0;
    tmp_basis[6] = normalized[2];
    tmp_basis[7] = 0;
    tmp_basis[8] = 0;

    Render_DrawRoomBuildRotMatrix(D_800C89F8, tmp_basis, out_basis);
    translation[0] = S16_AT(actor, 0x2E);
    translation[1] = U32_AT(PTR_AT(actor, 0x238) + U8_AT(actor, 0x27C) * 0x20, 0x14);
    translation[2] = U32_AT(PTR_AT(actor, 0x238) + U8_AT(actor, 0x27C) * 0x20, 0x1C);
}

void func_800655D4(void) {
}

int Render_DrawRoom(u8 *actor) {
    s32 room_matrix[8];
    s16 basis[9];
    s16 out_basis[9];
    s32 translation[3];
    RoomVec point;
    s16 radius;

    if ((U32_AT(actor, 0x98) & 0x400) != 0) {
        return 0;
    }
    if (U8_AT(actor, 0x252) == 0) {
        return 0;
    }

    point.x = S16_AT(actor, 0x2A);
    point.y = S16_AT(actor, 0x2E);
    point.z = S16_AT(actor, 0x32);
    point.pad = 0;

    room_matrix[0] = U32_AT(PTR_AT(actor, 0x238) + U8_AT(actor, 0x27C) * 0x20, 0x00);
    room_matrix[1] = U32_AT(PTR_AT(actor, 0x238) + U8_AT(actor, 0x27C) * 0x20, 0x04);
    room_matrix[2] = U32_AT(PTR_AT(actor, 0x238) + U8_AT(actor, 0x27C) * 0x20, 0x08);
    room_matrix[3] = U32_AT(PTR_AT(actor, 0x238) + U8_AT(actor, 0x27C) * 0x20, 0x0C);
    room_matrix[4] = U32_AT(PTR_AT(actor, 0x238) + U8_AT(actor, 0x27C) * 0x20, 0x10);
    room_matrix[5] = 0;
    room_matrix[6] = 0;
    room_matrix[7] = 0;

    basis[0] = 0;
    basis[1] = 0;
    basis[2] = 0;
    basis[3] = 0;
    basis[4] = 0;
    basis[5] = 0;
    basis[6] = 0;
    basis[7] = 0;
    basis[8] = 0x1000;

    Render_DrawRoomBuildBasis(actor, room_matrix, basis, out_basis, translation);
    Render_DrawRoomBuildNormalMatrix(out_basis, out_basis);

    if ((U32_AT(actor, 0x98) & 0x04000000) != 0) {
        translation[0] = D_800942EC;
    } else {
        translation[0] = point.y;
    }

    SetRotMatrix((s32 *)out_basis);
    Render_DrawRoomLoadTrans(translation);
    SetGeomScreen(D_800C8A18);

    radius = U16_AT(PTR_AT(actor, 0x1B4), 0x14);
    Render_DrawRoomDrawBillboard(actor, &point, radius);
    return 0;
}
