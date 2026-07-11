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

typedef struct AnimVec {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} AnimVec;

extern s32 *D_800BCFA4;
extern u32 D_800B1638[];
extern s32 D_800A6360[];

void RotMatrixY(int angle, s16 *matrix);
void SetDrawMode(void *p, int dfe, int dtd, int tpage);

static void Render_AnimLoadRotMatrix(s32 *matrix) {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;

    w0 = matrix[0];
    w1 = matrix[1];
    w2 = matrix[2];
    w3 = matrix[3];
    w4 = matrix[4];

    asm volatile("ctc2 %0,$0" : : "r"(w0));
    asm volatile("ctc2 %0,$1" : : "r"(w1));
    asm volatile("ctc2 %0,$2" : : "r"(w2));
    asm volatile("ctc2 %0,$3" : : "r"(w3));
    asm volatile("ctc2 %0,$4" : : "r"(w4));
}

static void Render_AnimLoadTrans(s32 *matrix) {
    int w5;
    int w6;
    int w7;

    w5 = matrix[5];
    w6 = matrix[6];
    w7 = matrix[7];

    asm volatile("ctc2 %0,$5" : : "r"(w5));
    asm volatile("ctc2 %0,$6" : : "r"(w6));
    asm volatile("ctc2 %0,$7" : : "r"(w7));
}

static void Render_AnimLoadFullMatrix(s32 *matrix) {
    Render_AnimLoadRotMatrix(matrix);
    Render_AnimLoadTrans(matrix);
}

static void Render_AnimTransformAxis(u16 *src, s16 *dst) {
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

static void Render_AnimTransformTranslation(s32 *src, s32 *dst) {
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

static void Render_AnimBuildMatrix(s32 *view_matrix, s16 *local_matrix, s32 *pos, s32 *out_matrix) {
    Render_AnimLoadRotMatrix(view_matrix);
    Render_AnimTransformAxis((u16 *)local_matrix, (s16 *)out_matrix);
    Render_AnimTransformAxis((u16 *)local_matrix + 1, (s16 *)out_matrix + 1);
    Render_AnimTransformAxis((u16 *)local_matrix + 2, (s16 *)out_matrix + 2);
    Render_AnimLoadTrans(view_matrix);
    Render_AnimTransformTranslation(pos, out_matrix + 5);
}

static void Render_AnimProjectVertex(AnimVec *src, u32 *screen_out, s32 *depth_out) {
    int depth;

    asm volatile("lwc2 $0,0(%0)" : : "r"(src));
    asm volatile("lwc2 $1,4(%0)" : : "r"(src));
    asm volatile("nop");
    asm volatile("nop");
    asm volatile(".word 0x4A180001");
    asm volatile("swc2 $14,0(%0)" : : "r"(screen_out) : "memory");
    asm volatile("mfc2 %0,$19" : "=r"(depth));
    *depth_out = depth;
}

static s32 Render_AnimNclip(u32 xy0, u32 xy1, u32 xy2) {
    s32 mac0;

    asm volatile("mtc2 %0,$12" : : "r"(xy0));
    asm volatile("mtc2 %0,$13" : : "r"(xy1));
    asm volatile("mtc2 %0,$14" : : "r"(xy2));
    asm volatile("nop");
    asm volatile("nop");
    asm volatile(".word 0x4B400006");
    asm volatile("swc2 $24,0(%0)" : : "r"(&mac0) : "memory");
    return mac0;
}

static void Render_AnimLinkPrim(u8 *prim, u32 *ot, s32 depth) {
    u32 old;
    u32 prim_word;

    if (depth >= 0x1000) {
        return;
    }

    ot += depth;
    old = *ot;
    prim_word = U32_AT(prim, 0);
    U32_AT(prim, 0) = (prim_word & 0xFF000000) | (old & 0x00FFFFFF);
    *ot = (old & 0xFF000000) | ((u32)prim & 0x00FFFFFF);
}

static s32 Render_AnimDepth3(int a, int b) {
    s32 depth;

    depth = D_800A6360[a] + D_800A6360[b];
    if (depth < 0) {
        depth += 7;
    }
    return depth >> 3;
}

static void Render_AnimSetFlatTri(u8 *prim, u32 xy0, u32 xy1, u8 flags) {
    U8_AT(prim, 3) = 4;
    U8_AT(prim, 7) = (flags & 0x20) ? 0x52 : 0x50;
    U8_AT(prim, 4) = 0;
    U8_AT(prim, 5) = 0xFF;
    U8_AT(prim, 6) = 0xFF;
    U8_AT(prim, 0x0C) = 0;
    U8_AT(prim, 0x0D) = 0xFF;
    U8_AT(prim, 0x0E) = 0xFF;
    U32_AT(prim, 8) = xy0;
    U32_AT(prim, 0x10) = xy1;
}

static void Render_AnimSetSortedTri(u8 *prim, u32 near_xy, u32 far_xy, int red_green) {
    U8_AT(prim, 3) = 4;
    U8_AT(prim, 7) = 0x50;
    if (red_green) {
        U8_AT(prim, 4) = 0;
        U8_AT(prim, 5) = 0x80;
        U8_AT(prim, 6) = 0;
        U8_AT(prim, 0x0C) = 0;
        U8_AT(prim, 0x0D) = 0x80;
        U8_AT(prim, 0x0E) = 0;
    } else {
        U8_AT(prim, 4) = 0;
        U8_AT(prim, 5) = 0xFF;
        U8_AT(prim, 6) = 0xFF;
        U8_AT(prim, 0x0C) = 0;
        U8_AT(prim, 0x0D) = 0xFF;
        U8_AT(prim, 0x0E) = 0xFF;
    }
    U32_AT(prim, 8) = near_xy;
    U32_AT(prim, 0x10) = far_xy;
}

static u8 *Render_AnimEmitDrawModeIfNeeded(u8 *prim, u32 *ot, s32 depth, u8 flags) {
    if ((flags & 0x20) == 0) {
        return prim;
    }

    SetDrawMode(prim, 0, 1, (flags & 3) << 5);
    Render_AnimLinkPrim(prim, ot, depth);
    return prim + 8;
}

static u8 *Render_AnimEmitEdge(u8 *prim, u32 *ot, int a, int b, u8 flags, int red_green) {
    s32 depth;

    Render_AnimSetSortedTri(prim, D_800B1638[a], D_800B1638[b], red_green);
    depth = Render_AnimDepth3(a, b);
    if (depth < 0x1000) {
        Render_AnimLinkPrim(prim, ot, depth);
        prim += 0x14;
    }

    prim = Render_AnimEmitDrawModeIfNeeded(prim, ot, depth, flags);
    return prim;
}

static u8 *Render_AnimEmitFace(u8 *frame, u8 *prim, u32 *ot, u8 *face, u8 *flags_table) {
    u8 ia;
    u8 ib;
    u8 ic;
    u8 flags;
    s32 nclip;

    ia = face[0];
    ib = face[1];
    ic = face[2];
    nclip = Render_AnimNclip(D_800B1638[ia], D_800B1638[ib], D_800B1638[ic]);
    flags = flags_table[U8_AT(frame + U16_AT(frame, 6) * 4, face[3] * 4 + 3)];

    if (nclip <= 0) {
        s16 za;
        s16 zb;
        s16 zc;
        int first;
        int second;

        za = S16_AT(frame + 0x1C + ia * 8, 2);
        zb = S16_AT(frame + 0x1C + ib * 8, 2);
        zc = S16_AT(frame + 0x1C + ic * 8, 2);
        first = -1;
        second = -1;
        if (za == 0 && zb == 0) {
            first = ia;
            second = ib;
        } else if (za == 0 && zc == 0) {
            first = ia;
            second = ic;
        } else if (zb == 0 && zc == 0) {
            first = ib;
            second = ic;
        }

        if (first >= 0 && second >= 0) {
            Render_AnimSetFlatTri(prim, D_800B1638[first], D_800B1638[second], flags);
            prim = Render_AnimEmitEdge(prim, ot, first, second, flags, 0);
        } else {
            U32_AT(prim, 0) = 0;
            prim += 4;
        }
        return prim;
    }

    prim = Render_AnimEmitEdge(prim, ot, ia, ib, flags, 1);
    prim = Render_AnimEmitEdge(prim, ot, ia, ic, flags, 1);
    prim = Render_AnimEmitEdge(prim, ot, ib, ic, flags, 1);
    return prim;
}

int Render_DecompressAnimFrame(u8 *frame, s16 *position, int scale, int angle, u32 *ot, u8 *prim) {
    s16 local_matrix[9];
    s32 matrix[8];
    s32 pos[3];
    AnimVec *vertex;
    u8 *face;
    u8 *flags_table;
    u8 *prim_start;
    int vertex_count;
    int face_count;
    int i;
    int fixed_scale;

    prim_start = prim;
    fixed_scale = (scale >> 16);
    if (fixed_scale < 0) {
        fixed_scale += 3;
    }
    fixed_scale >>= 2;

    local_matrix[0] = fixed_scale;
    local_matrix[1] = 0;
    local_matrix[2] = 0;
    local_matrix[3] = 0;
    local_matrix[4] = fixed_scale;
    local_matrix[5] = 0;
    local_matrix[6] = 0;
    local_matrix[7] = 0;
    local_matrix[8] = fixed_scale;

    pos[0] = position[1];
    pos[1] = position[3];
    pos[2] = position[5];

    RotMatrixY(angle & 0xFFFF, local_matrix);
    Render_AnimBuildMatrix(D_800BCFA4, local_matrix, pos, matrix);
    Render_AnimLoadFullMatrix(matrix);

    vertex_count = U8_AT(frame, 1);
    vertex = (AnimVec *)(frame + 0x1C);
    for (i = 0; i < vertex_count; i++, vertex++) {
        Render_AnimProjectVertex(vertex, &D_800B1638[i], &D_800A6360[i]);
    }

    face = frame + 0x1C + vertex_count * 8;
    face += (U8_AT(frame, 8) + U8_AT(frame, 9)) * 4;
    face += (U8_AT(frame, 10) + U8_AT(frame, 11)) * 4;
    face += (U8_AT(frame, 12) + U8_AT(frame, 13) + U8_AT(frame, 14) + U8_AT(frame, 15)) * 8;
    face += U8_AT(frame, 16) * 4;
    face += U8_AT(frame, 17) * 2;
    face += U8_AT(frame, 18) * 2;

    flags_table = frame + U16_AT(frame, 6) * 4;
    for (i = 0; i < U8_AT(frame, 19); i++) {
        face[i * 2] &= 0x7F;
    }

    face_count = U8_AT(frame, 10);
    for (i = 0; i < face_count; i++, face += 4) {
        prim = Render_AnimEmitFace(frame, prim, ot, face, flags_table);
    }

    return prim - prim_start;
}
